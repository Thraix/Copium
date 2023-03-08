#include "copium/renderer/Renderer.h"

#include "copium/core/SwapChain.h"

namespace Copium
{
  static constexpr int QUAD_COUNT = 10000;
  static constexpr int MAX_NUM_VERTICES = 4 * QUAD_COUNT;
  static constexpr int MAX_NUM_INDICES = 6 * QUAD_COUNT;
  static constexpr int MAX_NUM_TEXTURES = 32;

  Renderer::Renderer(Vulkan& vulkan, VkRenderPass renderPass, DescriptorPool& descriptorPool)
    : vulkan{vulkan}, 
      descriptorPool{vulkan},
      ibo{vulkan, MAX_NUM_INDICES}, 
      emptyTexture{vulkan, {0, 0, 0, 0}, 1, 1},
      samplers{MAX_NUM_TEXTURES, &emptyTexture}
  {
    CP_ASSERT(MAX_NUM_INDICES < std::numeric_limits<uint16_t>::max(), "Renderer : Maximum number of indices too big");


    std::vector<uint16_t> indices;
    indices.resize(MAX_NUM_INDICES);
    for (int i = 0; i < QUAD_COUNT; i++)
    {
      indices[i * 6] = i * 4;
      indices[i * 6 + 1] = i * 4 + 1;
      indices[i * 6 + 2] = i * 4 + 2;
      indices[i * 6 + 3] = i * 4;
      indices[i * 6 + 4] = i * 4 + 2;
      indices[i * 6 + 5] = i * 4 + 3;
    }
    ibo.UpdateStaging(indices.data());
    PipelineCreator creator{renderPass, "res/shaders/renderer.vert", "res/shaders/renderer.frag"};
    creator.SetVertexDescriptor(Vertex::GetDescriptor());
    creator.AddDescriptorSetLayoutBinding(0, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, MAX_NUM_TEXTURES, VK_SHADER_STAGE_FRAGMENT_BIT);
    creator.SetDepthTest(false);
    graphicsPipeline = std::make_unique<Pipeline>(vulkan, creator);
  }

  void Renderer::Quad(const glm::vec2& from, const glm::vec2& to, const glm::vec3& color)
  {
    AllocateQuad();
    AddVertex(from, color, -1, glm::vec2{0, 0});
    AddVertex(glm::vec2{to.x, from.y}, color, -1, glm::vec2{0, 0});
    AddVertex(to, color, -1, glm::vec2{0, 0});
    AddVertex(glm::vec2{from.x, to.y}, color, -1, glm::vec2{0, 0});
  }


  void Renderer::Quad(const glm::vec2& from, const glm::vec2& to, const Sampler& sampler, const glm::vec2& texCoord1, const glm::vec2& texCoord2)
  {
    AllocateQuad();
    int texIndex = AllocateSampler(sampler);
    AddVertex(from, glm::vec3{1,1,1}, texIndex, texCoord1);
    AddVertex(glm::vec2{to.x, from.y}, glm::vec3{1,1,1}, texIndex, glm::vec2{texCoord2.x, texCoord1.y});
    AddVertex(to, glm::vec3{1,1,1}, texIndex, texCoord2);
    AddVertex(glm::vec2{from.x, to.y}, glm::vec3{1,1,1}, texIndex, glm::vec2{texCoord1.x, texCoord2.y});
  }

  void Renderer::AddVertex(const glm::vec2& position, const glm::vec3& color, int texindex, const glm::vec2& texCoord)
  {
    Vertex* vertex = (Vertex*)mappedVertexBuffer;
    vertex->position = position;
    vertex->color = color;
    vertex->texCoord = texCoord;
    vertex->texIndex = texindex;
    mappedVertexBuffer = (Vertex*)mappedVertexBuffer + 1;
  }

  void Renderer::Begin(CommandBuffer& commandBuffer)
  {
    graphicsPipeline->Bind(commandBuffer);
    ibo.Bind(commandBuffer);
    vboIndex = -1;
    NextVertexBuffer();
    currentCommandBuffer = &commandBuffer;
  }

  void Renderer::End()
  {
    Flush();
  }

  int Renderer::AllocateSampler(const Sampler& sampler)
  {
    for (size_t i = 0; i < texturesUsed; i++)
    {
      if (*samplers[i] == sampler)
      {
        return i;
      }
    }
    if (texturesUsed == MAX_NUM_TEXTURES)
    {
      Flush();
      NextVertexBuffer();
    }
    currentDescriptorSet->AddSampler(sampler, 0, vulkan.GetSwapChain().GetFlightIndex(), texturesUsed);
    samplers[texturesUsed] = &sampler;
    texturesUsed++;
    return texturesUsed - 1;
  }

  void Renderer::AllocateQuad()
  {
    if (vertexCount + 4 > MAX_NUM_VERTICES)
    {
      Flush();
      NextVertexBuffer();
    }
    vertexCount += 4;
    indexCount += 6;
  }

  void Renderer::Flush()
  {
    currentVertexBuffer->Unmap();
    VkBuffer buffer = *currentVertexBuffer;
    VkDeviceSize offset = currentVertexBuffer->GetPosition(vulkan.GetSwapChain().GetFlightIndex());
    vkCmdBindVertexBuffers(*currentCommandBuffer, 0, 1, &buffer, &offset);
    graphicsPipeline->SetDescriptorSet(0, *currentDescriptorSet);
    graphicsPipeline->BindDescriptorSets(*currentCommandBuffer);
    ibo.Draw(*currentCommandBuffer, indexCount);
  }

  void Renderer::NextVertexBuffer()
  {
    vboIndex++;
    if (vboIndex >= vbos.size())
    {
      // Allocate new buffer since all existing buffers are full
      vbos.emplace_back(std::make_unique<Buffer>(vulkan, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Vertex) * MAX_NUM_VERTICES, SwapChain::MAX_FRAMES_IN_FLIGHT));
      descriptorSets.emplace_back(std::make_unique<DescriptorSet>(vulkan, descriptorPool, graphicsPipeline->GetDescriptorSetLayout(0)));
      descriptorSets.back()->AddSamplers(samplers, 0);
    }
    currentVertexBuffer = vbos[vboIndex].get();
    currentDescriptorSet = descriptorSets[vboIndex].get();
    mappedVertexBuffer = (char*)currentVertexBuffer->Map() + currentVertexBuffer->GetPosition(vulkan.GetSwapChain().GetFlightIndex());
    vertexCount = 0;
    indexCount = 0;
    texturesUsed = 0;
  }
}