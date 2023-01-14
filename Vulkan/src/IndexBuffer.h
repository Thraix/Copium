#pragma once

#include "Buffer.h"

class IndexBuffer : public Buffer
{
  CP_DELETE_COPY_AND_MOVE_CTOR(IndexBuffer);
private:
  int indexCount;
public:
  IndexBuffer(Instance& instance, int indexCount)
    : Buffer{instance, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexCount * sizeof(uint16_t), 1}, indexCount{indexCount}
  {}

  void Bind(VkCommandBuffer commandBuffer) override
  {
    vkCmdBindIndexBuffer(commandBuffer, handle, 0, VK_INDEX_TYPE_UINT16);
  }

  void Draw(VkCommandBuffer commandBuffer)
  {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  }
};