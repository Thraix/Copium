#pragma once

#include "copium/asset/AssetMeta.h"
#include "copium/asset/AssetRef.h"
#include "copium/buffer/RendererVertexBuffer.h"
#include "copium/pipeline/DescriptorSet.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/sampler/Sampler.h"

namespace Copium
{
  class Batch
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Batch);
  private:
    RendererVertexBuffer vertexBuffer;
    std::unique_ptr<DescriptorSet> descriptorSet;
  public:
    Batch(AssetRef<Pipeline>& pipeline, DescriptorPool& descriptorPool, int vertexCount, const std::vector<const Sampler*> samplers);
    RendererVertexBuffer& GetVertexBuffer();
    DescriptorSet& GetDescriptorSet();
  };
}
