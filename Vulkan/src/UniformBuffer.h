#pragma once

#include "Common.h"
#include "Buffer.h"
#include <vulkan/vulkan.hpp>

class UniformBuffer : public Buffer
{
	CP_DELETE_COPY_AND_MOVE_CTOR(UniformBuffer);

public:
  UniformBuffer(Instance& instance, VkDeviceSize size)
    : Buffer{instance, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size, instance.GetMaxFramesInFlight()}
  {}

  template <typename T>
  void Update(const T& t)
  {
    CP_ASSERT(sizeof(T) == Buffer::GetSize(), "Template size is not the same as buffer size %u != %u", sizeof(T), Buffer::GetSize());
    Buffer::Update((void*)&t, instance.GetFlightIndex());
  }

  VkDescriptorBufferInfo GetDescriptorBufferInfo(int index) const
  {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = handle;
    bufferInfo.offset = (VkDeviceSize)index * size;
		bufferInfo.range = size;
    return bufferInfo;
  }
};
