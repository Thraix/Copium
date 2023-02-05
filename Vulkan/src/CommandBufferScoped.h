#pragma once

#include "Common.h"
#include "CommandBuffer.h"

namespace Copium
{
  class CommandBufferScoped : public CommandBuffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(CommandBufferScoped);
  public:
    CommandBufferScoped(Instance& instance)
      : CommandBuffer{instance, Type::SingleUse}
    {
      CommandBuffer::Begin();
    }

    ~CommandBufferScoped()
    {
      CommandBuffer::End();
      CommandBuffer::Submit();
    }
  };
}
