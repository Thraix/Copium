#include "copium/buffer/CommandBufferScoped.h"

namespace Copium
{
  CommandBufferScoped::CommandBufferScoped()
    : CommandBuffer{Type::SingleUse}
  {
    CommandBuffer::Begin();
  }

  CommandBufferScoped::~CommandBufferScoped()
  {
    CommandBuffer::End();
    CommandBuffer::Submit();
  }
}
