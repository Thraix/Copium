#pragma once

#include "Common.h"
#include "Instance.h"
#include <optional>
#include <vulkan/vulkan.hpp>

class Buffer
{
private:
  Instance& instance;

	VkDeviceMemory memory;
	VkBuffer handle;
	VkDeviceSize size;
	int count;

  void* mappedData = nullptr;

public:
	Buffer(Instance& instance, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size, int count)
    : instance{instance}, size{size}, count{count}
	{
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size * (VkDeviceSize)count;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VK_ASSERT(vkCreateBuffer(instance.GetDevice(), &createInfo, nullptr, &handle), "Failed to initialize buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(instance.GetDevice(), handle, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = FindMemoryType(instance, memoryRequirements.memoryTypeBits, properties);

    VK_ASSERT(vkAllocateMemory(instance.GetDevice(), &allocateInfo, nullptr, &memory), "Failed to allocate buffer memory");

    vkBindBufferMemory(instance.GetDevice(), handle, memory, 0);
	}

  ~Buffer()
  {
    vkFreeMemory(instance.GetDevice(), memory, nullptr);
    vkDestroyBuffer(instance.GetDevice(), handle, nullptr);
  }

	void Update(void* indexData, int index)
	{
    ASSERT(index >= 0 && index < count, "instance is outside of the buffer");

    if (mappedData == nullptr)
    {
			void* data;
      vkMapMemory(instance.GetDevice(), memory, index * size, size, 0, &data);
      memcpy(data, indexData, size);
      vkUnmapMemory(instance.GetDevice(), memory);
    }
    else
    {
      memcpy((char*)mappedData + index * size, indexData, size);
    }
	}

  void Map()
  {
    ASSERT(mappedData == nullptr, "Mapping an already mapped buffer")
		vkMapMemory(instance.GetDevice(), memory, 0, size * count, 0, &mappedData);
  }

  void Unmap()
  {
    ASSERT(mappedData != nullptr, "Unmapping an already unmapped buffer")

		vkUnmapMemory(instance.GetDevice(), memory);
		mappedData = nullptr;
  }

  VkBuffer GetHandle() const
  {
    return handle;
  }

  VkDescriptorBufferInfo GetDescriptorBufferInfo(int instance)
  {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = handle;
    bufferInfo.offset = (VkDeviceSize)instance * size;
		bufferInfo.range = size;
    return bufferInfo;
  }

  VkDeviceSize GetSize() const
  {
    return size;
  }

  VkDeviceSize GetPosition(int index) const
  {
    ASSERT(index >= 0 && index < count, "Instance is outside of the buffer");
    return size * (VkDeviceSize)index;
  }

  static void CopyBuffer(Instance& instance, const Buffer& srcBuffer, const Buffer& dstBuffer)
  {
    ASSERT(srcBuffer.size == dstBuffer.size && srcBuffer.count == dstBuffer.count, "Buffers have different sizes");
    VkCommandBufferAllocateInfo allocateInfo{};

    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = instance.GetCommandPool();
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    VK_ASSERT(vkAllocateCommandBuffers(instance.GetDevice(), &allocateInfo, &commandBuffer), "Failed to initialize command buffer");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy bufferCopy{};
    bufferCopy.dstOffset = 0;
    bufferCopy.srcOffset = 0;
    bufferCopy.size = srcBuffer.size * (VkDeviceSize)srcBuffer.count;

    vkCmdCopyBuffer(commandBuffer, srcBuffer.GetHandle(), dstBuffer.GetHandle(), 1, &bufferCopy);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(instance.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(instance.GetGraphicsQueue());

    vkFreeCommandBuffers(instance.GetDevice(), instance.GetCommandPool(), 1, &commandBuffer);
  }

private:
	static uint32_t FindMemoryType(Instance& instance, uint32_t typeFilter, VkMemoryPropertyFlags properties) 
  {
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(instance.GetPhysicalDevice(), &memoryProperties);
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i)
    {
      if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
        return i;
    }
    throw std::runtime_error("Failed to find suitable memory type");
	}
};
