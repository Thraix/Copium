#pragma once

#include "copium/asset/AssetMeta.h"
#include "copium/asset/AssetRef.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/buffer/IndexBuffer.h"
#include "copium/buffer/RendererVertexBuffer.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/renderer/Batch.h"
#include "copium/sampler/Texture2D.h"
#include "copium/sampler/Font.h"
#include "copium/util/Common.h"

#include <glm/glm.hpp>
#include <vector>

namespace Copium
{
  class Renderer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Renderer);
  private:
    DescriptorPool descriptorPool;
    IndexBuffer ibo;
    AssetRef<Pipeline> pipeline;
    std::vector<std::unique_ptr<Batch>> batches;

    // Temporary data during a render
    CommandBuffer* currentCommandBuffer;
    std::vector<const Sampler*> samplers;
    int batchIndex;
    int quadCount;
    int textureCount;
    void* mappedVertexBuffer;
  public:
    Renderer(const AssetRef<Pipeline>& pipeline);
    ~Renderer();

    void Quad(const glm::vec2& from, const glm::vec2& to, const glm::vec3& color = glm::vec3{1, 1, 1});
    void Quad(const glm::vec2& from, const glm::vec2& to, const Sampler& sampler, const glm::vec2& texCoord1 = glm::vec2{0, 0}, const glm::vec2& texCoord2 = glm::vec2{1, 1});
    // Returns the position where the text rendering ended
    glm::vec2 Text(const std::string& str, const glm::vec2& position, const Font& font, float size, const glm::vec3& color = glm::vec3(1, 1, 1));

    void Begin(CommandBuffer& commandBuffer);
    void End();

    Pipeline& GetGraphicsPipeline();
    void SetDescriptorSet(const DescriptorSet& descriptorSet);
  private:
    void InitializeIndexBuffer();

    int AllocateSampler(const Sampler& sampler);
    void AllocateQuad();
    void Flush();
    void NextBatch();

    void AddVertex(const glm::vec2& position, const glm::vec3& color, int texindex, const glm::vec2& texCoord, int type);
  };
}
