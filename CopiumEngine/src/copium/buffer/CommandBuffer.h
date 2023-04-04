#pragma once

#include "copium/util/Common.h"

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
    std::vector<VkCommandBuffer> commandBuffers;
    const Type type;

  public:
    CommandBuffer(Type type);
    virtual ~CommandBuffer();

    void Begin();
    void End();
    void Submit();

    operator VkCommandBuffer() const;
  };
}
