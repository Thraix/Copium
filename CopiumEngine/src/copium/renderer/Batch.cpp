#include "copium/renderer/Batch.h"

#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  Batch::Batch(Pipeline& pipeline, DescriptorPool& descriptorPool, int vertexCount, const std::vector<const Sampler*> samplers)
    : pipeline{pipeline},
      vertexBuffer{RendererVertex::GetDescriptor(), vertexCount},
      descriptorSet{pipeline.CreateDescriptorSet(descriptorPool, 0)}
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
