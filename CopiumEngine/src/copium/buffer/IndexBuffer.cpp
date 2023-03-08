#include "copium/buffer/IndexBuffer.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  IndexBuffer::IndexBuffer(Vulkan& vulkan, int indexCount)
    : Buffer{vulkan, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexCount * sizeof(uint16_t), 1}, 
      indexCount{indexCount}
  {}

  void IndexBuffer::Bind(const CommandBuffer& commandBuffer)
  {
    vkCmdBindIndexBuffer(commandBuffer, handle, 0, VK_INDEX_TYPE_UINT16);
  }

  void IndexBuffer::Draw(const CommandBuffer& commandBuffer)
  {
    vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
  }

  void IndexBuffer::Draw(const CommandBuffer& commandBuffer, int indices)
  {
    CP_ASSERT(indices > 0 && indices <= indexCount, "Draw : amount of indices is out of range");
    vkCmdDrawIndexed(commandBuffer, indices, 1, 0, 0, 0);
  }
}