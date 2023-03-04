#include "copium/buffer/UniformBuffer.h"

#include "copium/core/SwapChain.h"

namespace Copium
{
  UniformBuffer::UniformBuffer(Vulkan& vulkan, VkDeviceSize size)
    : Buffer{vulkan, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size, SwapChain::MAX_FRAMES_IN_FLIGHT}
  {}

  VkDescriptorBufferInfo UniformBuffer::GetDescriptorBufferInfo(int index) const
  {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = handle;
    bufferInfo.offset = (VkDeviceSize)index * size;
    bufferInfo.range = size;
    return bufferInfo;
  }
}