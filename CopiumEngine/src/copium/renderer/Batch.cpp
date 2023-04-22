#include "copium/renderer/Batch.h"

#include "copium/asset/AssetManager.h"
#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  Batch::Batch(AssetHandle pipeline, DescriptorPool& descriptorPool, int vertexCount, const std::vector<const Sampler*> samplers)
    : vertexBuffer{RendererVertex::GetDescriptor(), vertexCount},
      descriptorSet{AssetManager::GetAsset<Pipeline>(pipeline).CreateDescriptorSet(descriptorPool, 0)}
  {
    descriptorSet->SetSamplers(samplers, 0);
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
