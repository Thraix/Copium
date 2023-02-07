#pragma once

#include "Common.h"
#include "Instance.h"

#include <vulkan/vulkan.hpp>

namespace Copium
{
  class CommandBuffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(CommandBuffer);
  public:
    enum class Type
    {
      SingleUse, Dynamic
    };
  private:
    Instance& instance;

    std::vector<VkCommandBuffer> commandBuffers;
    const Type type;
    VkCommandBuffer currentCommandBuffer{VK_NULL_HANDLE};

  public:
    CommandBuffer(Instance& instance, Type type);

    virtual ~CommandBuffer();

    void Begin();
    void End();
    void Submit();
    void SubmitAsGraphicsQueue();

    operator VkCommandBuffer() const;
  };
}
