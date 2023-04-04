#pragma once

#include "copium/buffer/Buffer.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/pipeline/VertexDescriptor.h"

namespace Copium
{
  class RendererVertexBuffer : public Buffer
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(RendererVertexBuffer);
  public:
    RendererVertexBuffer(const VertexDescriptor& descriptor, int vertexCount);

    void Bind(const CommandBuffer& commandBuffer);
  };
}
