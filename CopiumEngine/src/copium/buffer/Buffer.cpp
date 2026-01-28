#include "copium/buffer/Buffer.h"

#include "copium/core/Vulkan.h"

namespace Copium
{
  Buffer::Buffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size, int count)
    : size{size},
      count{count}
  {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size * (VkDeviceSize)count;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    CP_VK_ASSERT(vkCreateBuffer(Vulkan::GetDevice(), &createInfo, nullptr, &handle), "Failed to initialize buffer");

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(Vulkan::GetDevice(), handle, &memoryRequirements);

    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = Vulkan::GetDevice().FindMemoryType(memoryRequirements.memoryTypeBits, properties);

    CP_VK_ASSERT(vkAllocateMemory(Vulkan::GetDevice(), &allocateInfo, nullptr, &memory),
                 "Failed to allocate buffer memory");

    vkBindBufferMemory(Vulkan::GetDevice(), handle, memory, 0);
  }

  Buffer::~Buffer()
  {
    VkDeviceMemory memoryCpy = memory;
    VkBuffer handleCpy = handle;
    Vulkan::GetDevice().QueueIdleCommand(
      [memoryCpy, handleCpy]()
      {
        vkFreeMemory(Vulkan::GetDevice(), memoryCpy, nullptr);
        vkDestroyBuffer(Vulkan::GetDevice(), handleCpy, nullptr);
      });
  }

  void Buffer::Update(void* indexData, int index)
  {
    CP_ASSERT(index >= 0 && index < count, "Index is outside of the buffer");

    if (mappedData == nullptr)
    {
      void* data;
      vkMapMemory(Vulkan::GetDevice(), memory, index * size, size, 0, &data);
      memcpy(data, indexData, size);
      vkUnmapMemory(Vulkan::GetDevice(), memory);
    }
    else
    {
      memcpy((char*)mappedData + index * size, indexData, size);
    }
  }

  void Buffer::UpdateStaging(void* data)
  {
    VkDeviceSize bufferSize = size * count;
    Buffer stagingBuffer{VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         bufferSize,
                         1};

    stagingBuffer.Update(data, 0);

    CopyBuffer(stagingBuffer, *this, 0, bufferSize);
  }

  void Buffer::UpdateStaging(void* data, VkDeviceSize offset, VkDeviceSize size)
  {
    Buffer stagingBuffer{VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                         VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                         size,
                         1};

    stagingBuffer.Update(data, 0);

    CopyBuffer(stagingBuffer, *this, offset, size);
  }

  void* Buffer::Map()
  {
    CP_ASSERT(mappedData == nullptr, "Mapping an already mapped buffer");
    vkMapMemory(Vulkan::GetDevice(), memory, 0, size * count, 0, &mappedData);
    return mappedData;
  }

  void Buffer::Unmap()
  {
    CP_ASSERT(mappedData != nullptr, "Unmapping an already unmapped buffer");

    vkUnmapMemory(Vulkan::GetDevice(), memory);
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
    CP_ASSERT(index >= 0 && index < count, "Index is outside of the buffer");
    return size * (VkDeviceSize)index;
  }

  void Buffer::CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize offset, VkDeviceSize size)
  {
    VkCommandBufferAllocateInfo allocateInfo{};

    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = Vulkan::GetDevice().GetCommandPool();
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    CP_VK_ASSERT(vkAllocateCommandBuffers(Vulkan::GetDevice(), &allocateInfo, &commandBuffer),
                 "Failed to initialize command buffer");

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

    vkQueueSubmit(Vulkan::GetDevice().GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(Vulkan::GetDevice().GetGraphicsQueue());

    vkFreeCommandBuffers(Vulkan::GetDevice(), Vulkan::GetDevice().GetCommandPool(), 1, &commandBuffer);
  }
}
