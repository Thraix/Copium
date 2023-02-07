#include "copium/mesh/Mesh.h"

namespace Copium
{
  void Mesh::Bind(const CommandBuffer& commandBuffer)
  {
    indexBuffer->Bind(commandBuffer);
    vertexBuffer->Bind(commandBuffer);
  }

  void Mesh::Render(const CommandBuffer& commandBuffer)
  {
    indexBuffer->Draw(commandBuffer);
  }
}