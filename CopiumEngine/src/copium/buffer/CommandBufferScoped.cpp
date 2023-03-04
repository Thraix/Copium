#include "copium/buffer/CommandBufferScoped.h"

namespace Copium
{
  CommandBufferScoped::CommandBufferScoped(Vulkan& vulkan)
    : CommandBuffer{vulkan, Type::SingleUse}
  {
    CommandBuffer::Begin();
  }

  CommandBufferScoped::~CommandBufferScoped()
  {
    CommandBuffer::End();
    CommandBuffer::Submit();
  }
}
