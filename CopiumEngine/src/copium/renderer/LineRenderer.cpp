#include "copium/renderer/LineRenderer.h"

#include "copium/core/Vulkan.h"
#include "copium/pipeline/PipelineCreator.h"
#include "copium/renderer/LineVertex.h"

namespace Copium
{
  static constexpr int MAX_NUM_LINES = 30000;
  static constexpr int MAX_NUM_VERTICES = 2 * MAX_NUM_LINES;

  LineRenderer::LineRenderer(const AssetRef<Pipeline>& pipeline)
    : descriptorPool{pipeline.GetAsset().GetDescriptorSetCount() * SwapChain::MAX_FRAMES_IN_FLIGHT, 0},
      ibo{MAX_NUM_VERTICES},
      pipeline{pipeline},
      vertexBuffer{LineVertex::GetDescriptor(), MAX_NUM_VERTICES}
  {
    InitializeIndexBuffer();
  }

  LineRenderer::~LineRenderer() = default;

  void LineRenderer::Draw(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color)
  {
    AllocateLine();
    AddVertex(from, color);
    AddVertex(to, color);
  }

  void LineRenderer::AddVertex(const glm::vec3& position, const glm::vec3& color)
  {
    LineVertex* vertex = (LineVertex*)mappedVertexBuffer;
    vertex->pos = position;
    vertex->color = color;
    mappedVertexBuffer = (LineVertex*)mappedVertexBuffer + 1;
  }

  void LineRenderer::Begin(CommandBuffer& commandBuffer)
  {
    Pipeline& pl = pipeline.GetAsset();
    pl.Bind(commandBuffer);
    ibo.Bind(commandBuffer);
    mappedVertexBuffer = (char*)vertexBuffer.Map() + vertexBuffer.GetPosition(Vulkan::GetSwapChain().GetFlightIndex());
    lineCount = 0;
    currentCommandBuffer = &commandBuffer;
  }

  void LineRenderer::End()
  {
    Flush();
  }

  Pipeline& LineRenderer::GetGraphicsPipeline()
  {
    return pipeline.GetAsset();
  }

  void LineRenderer::SetDescriptorSet(const DescriptorSet& descriptorSet)
  {
    pipeline.GetAsset().SetDescriptorSet(descriptorSet);
  }

  void LineRenderer::InitializeIndexBuffer()
  {
    CP_ASSERT(MAX_NUM_VERTICES < std::numeric_limits<uint16_t>::max(), "Maximum number of indices too big");

    std::vector<uint16_t> indices;
    indices.resize(MAX_NUM_VERTICES);
    for (int i = 0; i < MAX_NUM_VERTICES; i++)
    {
      indices[i] = i;
    }
    ibo.UpdateStaging(indices.data());
  }

  void LineRenderer::AllocateLine()
  {
    CP_ASSERT(lineCount + 1 <= MAX_NUM_LINES, "Max number of lines reached in LineRenderer");
    lineCount++;
  }

  void LineRenderer::Flush()
  {
    vertexBuffer.Unmap();
    vertexBuffer.Bind(*currentCommandBuffer);
    Pipeline& pl = pipeline.GetAsset();
    pl.BindDescriptorSets(*currentCommandBuffer);
    ibo.Draw(*currentCommandBuffer, lineCount * 2);
  }
}
