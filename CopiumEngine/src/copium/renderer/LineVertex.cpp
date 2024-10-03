#include "copium/renderer/LineVertex.h"

namespace Copium
{
  VertexDescriptor LineVertex::GetDescriptor()
  {
    VertexDescriptor descriptor{};
    descriptor.AddAttribute(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LineVertex, pos), sizeof(LineVertex));
    descriptor.AddAttribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(LineVertex, color), sizeof(LineVertex));
    return descriptor;
  }
}