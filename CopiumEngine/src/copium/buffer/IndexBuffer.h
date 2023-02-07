#pragma once

#include "copium/buffer/Buffer.h"
#include "copium/core/Instance.h"
#include "copium/util/Common.h"

namespace Copium
{
  class IndexBuffer final : public Buffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(IndexBuffer);
  private:
    int indexCount;
  public:
    IndexBuffer(Instance& instance, int indexCount);

    void Bind(const CommandBuffer& commandBuffer);
    void Draw(const CommandBuffer& commandBuffer);
  };
}