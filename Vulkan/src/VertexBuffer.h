#pragma once

#include "Buffer.h"
#include "VertexDescriptor.h"

class VertexBuffer : public Buffer
{
  CP_DELETE_COPY_AND_MOVE_CTOR(VertexBuffer);
  std::map<uint32_t, VkDeviceSize> bindingOffsets;
  std::map<uint32_t, VkDeviceSize> bindingSizes;
public:
  VertexBuffer(Instance& instance, VkDeviceSize size)
    : Buffer{instance, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, size, 1}
  {}

  VertexBuffer(Instance& instance, const VertexDescriptor& descriptor, int vertexCount)
    : Buffer{instance, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, descriptor.GetVertexSize() * vertexCount, 1}
  {}

  void Bind(VkCommandBuffer commandBuffer) override
  {
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &handle, &offset);
  }

  void Update(uint32_t binding, void* data)
  {
    UpdateStaging(data, bindingOffsets[binding], bindingSizes[binding]);
  }
};
