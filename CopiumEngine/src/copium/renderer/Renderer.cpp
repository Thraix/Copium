#include "copium/renderer/Renderer.h"

#include "copium/core/SwapChain.h"
#include "copium/pipeline/PipelineCreator.h"
#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  static constexpr int MAX_NUM_QUADS = 10000;
  static constexpr int MAX_NUM_VERTICES = 4 * MAX_NUM_QUADS;
  static constexpr int MAX_NUM_INDICES = 6 * MAX_NUM_QUADS;
  static constexpr int MAX_NUM_TEXTURES = 32;

  Renderer::Renderer(Vulkan& vulkan, VkRenderPass renderPass, DescriptorPool& descriptorPool)
    : vulkan{vulkan}, 
      descriptorPool{vulkan},
      ibo{vulkan, MAX_NUM_INDICES}, 
      emptyTexture{vulkan, {1, 0, 0, 0}, 1, 1},
      samplers{MAX_NUM_TEXTURES, &emptyTexture}
  {
    InitializeIndexBuffer();
    InitializeGraphicsPipeline(renderPass);
  }

  void Renderer::Quad(const glm::vec2& pos, const glm::vec2& size, const glm::vec3& color)
  {
    AllocateQuad();
    AddVertex(glm::vec2{pos.x, pos.y + size.y}, color, -1, glm::vec2{0, 0});
    AddVertex(pos + size, color, -1, glm::vec2{0, 0});
    AddVertex(glm::vec2{pos.x + size.x, pos.y}, color, -1, glm::vec2{0, 0});
    AddVertex(pos, color, -1, glm::vec2{0, 0});
  }


  void Renderer::Quad(const glm::vec2& pos, const glm::vec2& size, const Sampler& sampler, const glm::vec2& texCoord1, const glm::vec2& texCoord2)
  {
    AllocateQuad();
    int texIndex = AllocateSampler(sampler);
    AddVertex(glm::vec2{pos.x, pos.y + size.y}, glm::vec3{1, 1, 1}, texIndex, glm::vec2{texCoord1.x, texCoord2.y});
    AddVertex(pos + size, glm::vec3{1,1,1}, texIndex, texCoord2);
    AddVertex(glm::vec2{pos.x + size.x, pos.y}, glm::vec3{1, 1, 1}, texIndex, glm::vec2{texCoord2.x, texCoord1.y});
    AddVertex(pos, glm::vec3{1,1,1}, texIndex, texCoord1);
  }

  void Renderer::AddVertex(const glm::vec2& position, const glm::vec3& color, int texindex, const glm::vec2& texCoord)
  {
    RendererVertex* vertex = (RendererVertex*)mappedVertexBuffer;
    vertex->position = position;
    vertex->color = color;
    vertex->texCoord = texCoord;
    vertex->texIndex = texindex;
    mappedVertexBuffer = (RendererVertex*)mappedVertexBuffer + 1;
  }

  void Renderer::Begin(CommandBuffer& commandBuffer)
  {
    graphicsPipeline->Bind(commandBuffer);
    ibo.Bind(commandBuffer);
    batchIndex = -1;
    NextBatch();
    currentCommandBuffer = &commandBuffer;
  }

  void Renderer::End()
  {
    Flush();
  }

  Pipeline& Renderer::GetGraphicsPipeline()
  {
    return *graphicsPipeline;
  }

  void Renderer::SetDescriptorSet(const DescriptorSet& descriptorSet)
  {
    graphicsPipeline->SetDescriptorSet(descriptorSet);
  }

  void Renderer::InitializeIndexBuffer() 
  {
    CP_ASSERT(MAX_NUM_INDICES < std::numeric_limits<uint16_t>::max(), "Renderer : Maximum number of indices too big");

    std::vector<uint16_t> indices;
    indices.resize(MAX_NUM_INDICES);
    for (int i = 0; i < MAX_NUM_QUADS; i++)
    {
      indices[i * 6] = i * 4;
      indices[i * 6 + 1] = i * 4 + 1;
      indices[i * 6 + 2] = i * 4 + 2;
      indices[i * 6 + 3] = i * 4;
      indices[i * 6 + 4] = i * 4 + 2;
      indices[i * 6 + 5] = i * 4 + 3;
    }
    ibo.UpdateStaging(indices.data());
  }

  void Renderer::InitializeGraphicsPipeline(VkRenderPass renderPass)
  {
    PipelineCreator creator{renderPass, "res/shaders/renderer.vert", "res/shaders/renderer.frag"};
    creator.SetVertexDescriptor(RendererVertex::GetDescriptor());
    creator.SetDepthTest(false);
    graphicsPipeline = std::make_unique<Pipeline>(vulkan, creator);
  }

  int Renderer::AllocateSampler(const Sampler& sampler)
  {
    for (size_t i = 0; i < textureCount; i++)
    {
      if (*samplers[i] == sampler)
      {
        return i;
      }
    }
    if (textureCount == MAX_NUM_TEXTURES)
    {
      Flush();
      NextBatch();
    }
    batches[batchIndex]->GetDescriptorSet().SetSamplerDynamic(sampler, 0, textureCount);
    samplers[textureCount] = &sampler;
    textureCount++;
    return textureCount - 1;
  }

  void Renderer::AllocateQuad()
  {
    if (quadCount + 1 > MAX_NUM_QUADS)
    {
      Flush();
      NextBatch();
    }
    quadCount++;
  }

  void Renderer::Flush()
  {
    batches[batchIndex]->GetVertexBuffer().Unmap();
    batches[batchIndex]->GetVertexBuffer().Bind(*currentCommandBuffer);
    graphicsPipeline->SetDescriptorSet(batches[batchIndex]->GetDescriptorSet());
    graphicsPipeline->BindDescriptorSets(*currentCommandBuffer);
    ibo.Draw(*currentCommandBuffer, quadCount * 6);
  }

  void Renderer::NextBatch()
  {
    batchIndex++;
    if (batchIndex >= batches.size())
    {
      batches.emplace_back(std::make_unique<Batch>(vulkan, *graphicsPipeline, descriptorPool, MAX_NUM_VERTICES, samplers));
    }
    mappedVertexBuffer = (char*)batches[batchIndex]->GetVertexBuffer().Map() + batches[batchIndex]->GetVertexBuffer().GetPosition(vulkan.GetSwapChain().GetFlightIndex());
    quadCount = 0;
    textureCount = 0;
    std::fill(samplers.begin(), samplers.end(), &emptyTexture);
  }
}