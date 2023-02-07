#pragma once

#include "Common.h"
#include "CommandBuffer.h"

namespace Copium
{
  class CommandBufferScoped final : public CommandBuffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(CommandBufferScoped);
  public:
    CommandBufferScoped(Instance& instance);

    ~CommandBufferScoped() override;
  };
}
