#pragma once

#include "copium/buffer/IndexBuffer.h"
#include "copium/buffer/VertexBuffer.h"
#include "copium/buffer/CommandBuffer.h"
#include "copium/util/Common.h"

#include <vector>
#include <memory>

namespace Copium
{
  class Mesh
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Mesh);
  private:
    std::unique_ptr<IndexBuffer> indexBuffer;
    std::unique_ptr<VertexBuffer> vertexBuffer;
  public:
    template <typename T>
    Mesh(const std::vector<T>& vertices, const std::vector<uint16_t>& indices);

    void Bind(const CommandBuffer& commandBuffer);
    void Render(const CommandBuffer& commandBuffer);
  };

  template <typename T>
  Mesh::Mesh(const std::vector<T>& vertices, const std::vector<uint16_t>& indices)
  {
    indexBuffer = std::make_unique<IndexBuffer>(indices.size());
    indexBuffer->UpdateStaging((void*)indices.data());
    vertexBuffer = std::make_unique<VertexBuffer>(T::GetDescriptor(), vertices.size());
    vertexBuffer ->UpdateStaging(0, (void*)vertices.data());
  }
}
