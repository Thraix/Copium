#include "copium/buffer/Buffer.h"

#include "copium/core/Device.h"
#include "copium/core/Instance.h"

namespace Copium
{
  Buffer::Buffer(Vulkan& vulkan, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size, int count)
    : vulkan{vulkan}, size{size}, count{count}
  {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size * (VkDeviceSize)count;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CP_VK_ASSERT(vkCreateBuffer(vulkan.GetDevice(), &createInfo, nullptr, &handle), "Buffer : Failed to initialize buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(vulkan.GetDevice(), handle, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = vulkan.GetDevice().FindMemoryType(memoryRequirements.memoryTypeBits, properties);

    CP_VK_ASSERT(vkAllocateMemory(vulkan.GetDevice(), &allocateInfo, nullptr, &memory), "Buffer : Failed to allocate buffer memory");

    vkBindBufferMemory(vulkan.GetDevice(), handle, memory, 0);
  }

  Buffer::~Buffer()
  {
    vkFreeMemory(vulkan.GetDevice(), memory, nullptr);
    vkDestroyBuffer(vulkan.GetDevice(), handle, nullptr);
  }

  void Buffer::Update(void* indexData, int index)
  {
    CP_ASSERT(index >= 0 && index < count, "Update : Index is outside of the buffer");

    if (mappedData == nullptr)
    {
      void* data;
      vkMapMemory(vulkan.GetDevice(), memory, index * size, size, 0, &data);
      memcpy(data, indexData, size);
      vkUnmapMemory(vulkan.GetDevice(), memory);
    }
    else
    {
      memcpy((char*)mappedData + index * size, indexData, size);
    }
  }

  void Buffer::UpdateStaging(void* data)
  {
    VkDeviceSize bufferSize = size * count;
    Buffer stagingBuffer{vulkan, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, bufferSize, 1};

    stagingBuffer.Update(data, 0);

    CopyBuffer(vulkan, stagingBuffer, *this, 0, bufferSize);
  }

  void Buffer::UpdateStaging(void* data, VkDeviceSize offset, VkDeviceSize size)
  {
    Buffer stagingBuffer{vulkan, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, size, 1};

    stagingBuffer.Update(data, 0);

    CopyBuffer(vulkan, stagingBuffer, *this, offset, size);
  }

  void* Buffer::Map()
  {
    CP_ASSERT(mappedData == nullptr, "Map : Mapping an already mapped buffer");
    vkMapMemory(vulkan.GetDevice(), memory, 0, size * count, 0, &mappedData);
    return mappedData;
  }

  void Buffer::Unmap()
  {
    CP_ASSERT(mappedData != nullptr, "Unmap : Unmapping an already unmapped buffer");

    vkUnmapMemory(vulkan.GetDevice(), memory);
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

  void Buffer::CopyBuffer(Vulkan& vulkan, const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize offset, VkDeviceSize size)
  {
    VkCommandBufferAllocateInfo allocateInfo{};

    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = vulkan.GetDevice().GetCommandPool();
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    CP_VK_ASSERT(vkAllocateCommandBuffers(vulkan.GetDevice(), &allocateInfo, &commandBuffer), "CopyBuffer : Failed to initialize command buffer");

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

    vkQueueSubmit(vulkan.GetDevice().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vulkan.GetDevice().GetGraphicsQueue());

    vkFreeCommandBuffers(vulkan.GetDevice(), vulkan.GetDevice().GetCommandPool(), 1, &commandBuffer);
  }
}
