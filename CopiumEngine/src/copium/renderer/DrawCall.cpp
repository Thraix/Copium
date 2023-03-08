#include "copium/renderer/DrawCall.h"

#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  DrawCall::DrawCall(Vulkan& vulkan, Pipeline& pipeline, DescriptorPool& descriptorPool, int vertexCount, const std::vector<const Sampler*> samplers)
    : vulkan{vulkan},
      pipeline{pipeline},
      vertexBuffer{vulkan, RendererVertex::GetDescriptor(), vertexCount},
      descriptorSet{vulkan, descriptorPool, pipeline.GetDescriptorSetLayout(0)}
  {
    descriptorSet.AddSamplers(samplers, 0);
  }

  RendererVertexBuffer& DrawCall::GetVertexBuffer()
  {
    return vertexBuffer;
  }

  DescriptorSet& DrawCall::GetDescriptorSet()
  {
    return descriptorSet;
  }
}
