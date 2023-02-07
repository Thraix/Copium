#pragma once

#include "copium/buffer/Buffer.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/core/Instance.h"
#include "copium/pipeline/VertexDescriptor.h"
#include "copium/util/Common.h"

#include <vector>
#include <vulkan/vulkan.hpp>

namespace Copium
{
  class VertexBuffer final : public Buffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(VertexBuffer);
  private:
    std::vector<VkDeviceSize> bindingOffsets;
    std::vector<VkDeviceSize> bindingSizes;
  public:
    VertexBuffer(Instance& instance, const VertexDescriptor& descriptor, int vertexCount);

    void Bind(const CommandBuffer& commandBuffer);
    void UpdateStaging(uint32_t binding, void* data);
  };
}
