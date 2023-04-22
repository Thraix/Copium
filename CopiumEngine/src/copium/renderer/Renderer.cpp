#include "copium/renderer/Renderer.h"

#include "copium/asset/AssetManager.h"
#include "copium/core/Vulkan.h"
#include "copium/pipeline/PipelineCreator.h"
#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  static constexpr int MAX_NUM_QUADS = 10000;
  static constexpr int MAX_NUM_VERTICES = 4 * MAX_NUM_QUADS;
  static constexpr int MAX_NUM_INDICES = 6 * MAX_NUM_QUADS;
  static constexpr int MAX_NUM_TEXTURES = 32;

  Renderer::Renderer()
    : descriptorPool{},
      ibo{MAX_NUM_INDICES}, 
      emptyTexture{AssetManager::RegisterRuntimeAsset("empty", std::make_unique<Texture2D>(std::vector<uint8_t>{0, 0, 0, 255}, 1, 1))},
      samplers{MAX_NUM_TEXTURES, &AssetManager::GetAsset<Texture2D>(emptyTexture)}
  {
    InitializeIndexBuffer();
    InitializeGraphicsPipeline();
  }

  Renderer::~Renderer()
  {
    AssetManager::UnloadAsset(emptyTexture);
    AssetManager::UnloadAsset(pipeline);
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
    AssetManager::GetAsset<Pipeline>(pipeline).Bind(commandBuffer);
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
    return AssetManager::GetAsset<Pipeline>(pipeline);
  }

  void Renderer::SetDescriptorSet(const DescriptorSet& descriptorSet)
  {
    AssetManager::GetAsset<Pipeline>(pipeline).SetDescriptorSet(descriptorSet);
  }

  void Renderer::InitializeIndexBuffer() 
  {
    CP_ASSERT(MAX_NUM_INDICES < std::numeric_limits<uint16_t>::max(), "Maximum number of indices too big");

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

  void Renderer::InitializeGraphicsPipeline()
  {
    pipeline = AssetManager::LoadAsset<Pipeline>("renderer.meta");
  }

  int Renderer::AllocateSampler(const Sampler& sampler)
  {
    for (size_t i = 0; i < textureCount; i++)
    {
      if (*samplers[i] == (VkSampler)sampler)
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
    AssetManager::GetAsset<Pipeline>(pipeline).SetDescriptorSet(batches[batchIndex]->GetDescriptorSet());
    AssetManager::GetAsset<Pipeline>(pipeline).BindDescriptorSets(*currentCommandBuffer);
    ibo.Draw(*currentCommandBuffer, quadCount * 6);
  }

  void Renderer::NextBatch()
  {
    batchIndex++;
    std::fill(samplers.begin(), samplers.end(), &AssetManager::GetAsset<Texture2D>(emptyTexture));
    if (batchIndex >= batches.size())
    {
      batches.emplace_back(std::make_unique<Batch>(pipeline, descriptorPool, MAX_NUM_VERTICES, samplers));
    }
    mappedVertexBuffer = (char*)batches[batchIndex]->GetVertexBuffer().Map() + batches[batchIndex]->GetVertexBuffer().GetPosition(Vulkan::GetSwapChain().GetFlightIndex());
    quadCount = 0;
    textureCount = 0;
  }
}