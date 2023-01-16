#pragma once

#include "Buffer.h"
#include "VertexDescriptor.h"

class VertexBuffer : public Buffer
{
  CP_DELETE_COPY_AND_MOVE_CTOR(VertexBuffer);
private:
  std::vector<VkDeviceSize> bindingOffsets;
  std::vector<VkDeviceSize> bindingSizes;
public:
  VertexBuffer(Instance& instance, const VertexDescriptor& descriptor, int vertexCount)
    : Buffer{instance, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, descriptor.GetVertexSize() * vertexCount, 1}
  {
    VkDeviceSize offset = 0;
    for (auto&& binding : descriptor.GetBindings())
    {
      bindingOffsets.emplace_back(offset);
      bindingSizes.emplace_back(binding.stride * vertexCount);
      offset += binding.stride * vertexCount;
    }
  }

  void Bind(VkCommandBuffer commandBuffer) override
  {
    std::vector<VkBuffer> buffers{bindingOffsets.size(), handle};
    vkCmdBindVertexBuffers(commandBuffer, 0, bindingOffsets.size(), buffers.data(), bindingOffsets.data());
  }

  void Update(uint32_t binding, void* data)
  {
    UpdateStaging(data, bindingOffsets[binding], bindingSizes[binding]);
  }
};
