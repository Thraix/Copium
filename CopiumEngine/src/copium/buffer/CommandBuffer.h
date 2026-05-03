#pragma once

#include <vulkan/vulkan.hpp>

#include "copium/util/Common.h"
#include "copium/util/Enum.h"

namespace Copium
{
  CP_ENUM_CREATOR(CommandBufferType, SingleUse, Dynamic);

  class CommandBuffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(CommandBuffer);

  private:
    std::vector<VkCommandBuffer> commandBuffers;
    const CommandBufferType type;

  public:
    CommandBuffer(CommandBufferType type);
    virtual ~CommandBuffer();

    void Begin();
    void End();
    void Submit();

    operator VkCommandBuffer() const;
  };
}
