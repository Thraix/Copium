#pragma once

#include "copium/buffer/Buffer.h"
#include "copium/core/Instance.h"
#include "copium/util/Common.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class UniformBuffer final : public Buffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(UniformBuffer);

  public:
    UniformBuffer(Instance& instance, VkDeviceSize size);

    VkDescriptorBufferInfo GetDescriptorBufferInfo(int index) const;

    template <typename T>
    void Update(const T& t);
  };

  template <typename T>
  void UniformBuffer::Update(const T& t)
  {
    CP_ASSERT(sizeof(T) == Buffer::GetSize(), "Update : Template size is not the same as buffer size %u != %u", sizeof(T), Buffer::GetSize());
    Buffer::Update((void*)&t, instance.GetFlightIndex());
  }
}
