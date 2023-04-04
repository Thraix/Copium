#pragma once

#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class Buffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Buffer);
  protected:
    VkDeviceMemory memory;
    VkBuffer handle;
    VkDeviceSize size;
    int count;

    void* mappedData = nullptr;

  public:
    Buffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size, int count);
    virtual ~Buffer();

    void Update(void* indexData, int index);
    void UpdateStaging(void* data);
    void UpdateStaging(void* data, VkDeviceSize offset, VkDeviceSize size);

    void* Map();
    void Unmap();

    operator VkBuffer() const;
    VkDeviceSize GetSize() const;
    VkDeviceSize GetPosition(int index) const;

    static void CopyBuffer(const Buffer& srcBuffer, const Buffer& dstBuffer, VkDeviceSize offset, VkDeviceSize size);
  };
}
