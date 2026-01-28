#include "copium/mesh/VertexPassthrough.h"

namespace Copium
{
  VertexDescriptor VertexPassthrough::GetDescriptor()
  {
    VertexDescriptor descriptor{};
    descriptor.AddAttribute(
      0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(VertexPassthrough, texCoord), sizeof(VertexPassthrough));
    return descriptor;
  }
}