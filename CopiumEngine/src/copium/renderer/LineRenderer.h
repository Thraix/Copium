#pragma once

#include <glm/glm.hpp>

#include "copium/buffer/CommandBuffer.h"
#include "copium/buffer/IndexBuffer.h"
#include "copium/buffer/RendererVertexBuffer.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/util/Common.h"

namespace Copium
{
  class LineRenderer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(LineRenderer);

  public:
    LineRenderer(const AssetRef<Pipeline>& pipeline);
    ~LineRenderer();

    void Draw(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color = glm::vec3{1, 1, 1});

    void Begin(CommandBuffer& commandBuffer);
    void End();

    Pipeline& GetGraphicsPipeline();
    void SetDescriptorSet(const DescriptorSet& descriptorSet);

  private:
    DescriptorPool descriptorPool;
    IndexBuffer ibo;
    AssetRef<Pipeline> pipeline;

    RendererVertexBuffer vertexBuffer;

    // Temporary data during a render
    CommandBuffer* currentCommandBuffer;
    int lineCount;
    void* mappedVertexBuffer;

  private:
    void InitializeIndexBuffer();

    void AllocateLine();
    void Flush();

    void AddVertex(const glm::vec3& position, const glm::vec3& color);
  };
}
