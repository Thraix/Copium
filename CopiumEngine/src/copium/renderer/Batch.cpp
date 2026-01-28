#include "copium/renderer/Batch.h"

#include "copium/core/SwapChain.h"
#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  Batch::Batch(AssetRef<Pipeline>& pipeline, int vertexCount, const std::vector<const Sampler*> samplers)
    : vertexBuffer{RendererVertex::GetDescriptor(), vertexCount},
      descriptorPool{pipeline.GetAsset().GetDescriptorSetCount() * SwapChain::MAX_FRAMES_IN_FLIGHT,
                     32 * SwapChain::MAX_FRAMES_IN_FLIGHT},
      descriptorSet{pipeline.GetAsset().CreateDescriptorSet(descriptorPool, 0)}
  {
  }

  RendererVertexBuffer& Batch::GetVertexBuffer()
  {
    return vertexBuffer;
  }

  DescriptorSet& Batch::GetDescriptorSet()
  {
    return *descriptorSet;
  }
}
