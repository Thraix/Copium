#pragma once

#include "Buffer.h"
#include "CommandBuffer.h"
#include "Instance.h"
#include "VertexDescriptor.h"

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
    void Update(uint32_t binding, void* data);
  };
}
