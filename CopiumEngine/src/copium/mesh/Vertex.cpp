#include "copium/mesh/Vertex.h"

namespace Copium
{
  VertexDescriptor Vertex::GetDescriptor()
  {
    VertexDescriptor descriptor{};
    descriptor.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos), sizeof(Vertex));
    descriptor.AddAttribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color), sizeof(Vertex));
    descriptor.AddAttribute(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord), sizeof(Vertex));
    return descriptor;
  }
}