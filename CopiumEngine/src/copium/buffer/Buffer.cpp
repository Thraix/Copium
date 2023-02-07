#include "copium/buffer/Buffer.h"

namespace Copium
{
  Buffer::Buffer(Instance& instance, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size, int count)
    : instance{instance}, size{size}, count{count}
  {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size * (VkDeviceSize)count;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CP_VK_ASSERT(vkCreateBuffer(instance.GetDevice(), &createInfo, nullptr, &handle), "Buffer : Failed to initialize buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(instance.GetDevice(), handle, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = instance.FindMemoryType(memoryRequirements.memoryTypeBits, properties);

    CP_VK_ASSERT(vkAllocateMemory(instance.GetDevice(), &allocateInfo, nullptr, &memory), "Buffer : Failed to allocate buffer memory");

    vkBindBufferMemory(instance.GetDevice(), handle, memory, 0);
  }

  Buffer::~Buffer()
  {
    vkFreeMemory(instance.GetDevice(), memory, nullptr);
    vkDestroyBuffer(instance.GetDevice(), handle, nullptr);
  }

  void Buffer::Update(void* indexData, int index)
  {
    CP_ASSERT(index >= 0 && index < count, "Update : Index is outside of the buffer");

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

  void Buffer::UpdateStaging(void* data)
  {
    VkDeviceSize bufferSize = size * count;
    Buffer stagingBuffer{instance, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, 1};

    stagingBuffer.Update(data, 0);

    CopyBuffer(instance, stagingBuffer, *this, 0, bufferSize);
  }

  void Buffer::UpdateStaging(void* data, VkDeviceSize offset, VkDeviceSize size)
  {
    Buffer stagingBuffer{instance, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size, 1};

    stagingBuffer.Update(data, 0);

    CopyBuffer(instance, stagingBuffer, *this, offset, size);
  }

  void* Buffer::Map()
  {
    CP_ASSERT(mappedData == nullptr, "Map : Mapping an already mapped buffer");
    vkMapMemory(instance.GetDevice(), memory, 0, size * count, 0, &mappedData);
    return mappedData;
  }

  void Buffer::Unmap()
  {
    CP_ASSERT(mappedData != nullptr, "Unmap : Unmapping an already unmapped buffer");

    vkUnmapMemory(instance.GetDevice(), memory);
    mappedData = nullptr;
  }

  Buffer::operator VkBuffer() const 
  {
    return handle;
  }

  VkDeviceSize Buffer::GetSize() const
  {
    return size;
  }

  VkDeviceSize Buffer::GetPosition(int index) const
  {
    CP_ASSERT(index >= 0 && index < count, "GetPosition : Index is outside of the buffer");
    return size * (VkDeviceSize)index;
  }

  void Buffer::CopyBuffer(Instance& instance, const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize offset, VkDeviceSize size)
  {
    VkCommandBufferAllocateInfo allocateInfo{};

    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = instance.GetCommandPool();
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    CP_VK_ASSERT(vkAllocateCommandBuffers(instance.GetDevice(), &allocateInfo, &commandBuffer), "CopyBuffer : Failed to initialize command buffer");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy bufferCopy{};
    bufferCopy.dstOffset = offset;
    bufferCopy.srcOffset = 0;
    bufferCopy.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(instance.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(instance.GetGraphicsQueue());

    vkFreeCommandBuffers(instance.GetDevice(), instance.GetCommandPool(), 1, &commandBuffer);
  }
}
