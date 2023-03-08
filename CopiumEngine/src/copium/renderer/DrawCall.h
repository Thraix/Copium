#pragma once

#include "copium/buffer/RendererVertexBuffer.h"
#include "copium/pipeline/DescriptorSet.h"
#include "copium/pipeline/Pipeline.h"
#include "copium/sampler/Sampler.h"

namespace Copium
{
  class DrawCall
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(DrawCall);
  private:
    Vulkan& vulkan;
    Pipeline& pipeline;

    RendererVertexBuffer vertexBuffer;
    DescriptorSet descriptorSet;
  public:
    DrawCall(Vulkan& vulkan, Pipeline& pipeline, DescriptorPool& descriptorPool, int vertexCount, const std::vector<const Sampler*> samplers);
    RendererVertexBuffer& GetVertexBuffer();
    DescriptorSet& GetDescriptorSet();
  };
}
