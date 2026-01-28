#pragma once

#include "copium/buffer/CommandBuffer.h"
#include "copium/util/Common.h"

namespace Copium
{
  class CommandBufferScoped final : public CommandBuffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(CommandBufferScoped);

  public:
    CommandBufferScoped();

    ~CommandBufferScoped() override;
  };
}
