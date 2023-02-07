#include "copium/buffer/CommandBufferScoped.h"

namespace Copium
{
  CommandBufferScoped::CommandBufferScoped(Instance& instance)
    : CommandBuffer{instance, Type::SingleUse}
  {
    CommandBuffer::Begin();
  }

  CommandBufferScoped::~CommandBufferScoped()
  {
    CommandBuffer::End();
    CommandBuffer::Submit();
  }
}
