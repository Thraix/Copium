#pragma once

#include "copium/buffer/Buffer.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/core/Vulkan.h"
#include "copium/util/Common.h"

namespace Copium
{
  class IndexBuffer final : public Buffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(IndexBuffer);
  private:
    int indexCount;
  public:
    IndexBuffer(Vulkan& vulkan, int indexCount);

    void Bind(const CommandBuffer& commandBuffer);
    void Draw(const CommandBuffer& commandBuffer);
    void Draw(const CommandBuffer& commandBuffer, int indices);
  };
}