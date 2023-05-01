#pragma once

#include "copium/util/Common.h"
#include "copium/util/Enum.h"

#include <vulkan/vulkan.hpp>

#define CP_COMMAND_BUFFER_TYPE_ENUMS SingleUse, Dynamic
CP_ENUM_CREATOR(Copium, CommandBufferType, CP_COMMAND_BUFFER_TYPE_ENUMS);

namespace Copium
{
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
