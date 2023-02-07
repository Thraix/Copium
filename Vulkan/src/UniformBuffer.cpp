#include "UniformBuffer.h"

namespace Copium
{
  UniformBuffer::UniformBuffer(Instance& instance, VkDeviceSize size)
    : Buffer{instance, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size, instance.GetMaxFramesInFlight()}
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