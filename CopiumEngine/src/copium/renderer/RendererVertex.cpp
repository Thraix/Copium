#include "copium/renderer/RendererVertex.h"

namespace Copium
{
  VertexDescriptor RendererVertex::GetDescriptor()
  {
    VertexDescriptor descriptor{};
    descriptor.AddAttribute(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(RendererVertex, position), sizeof(RendererVertex));
    descriptor.AddAttribute(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(RendererVertex, color), sizeof(RendererVertex));
    descriptor.AddAttribute(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(RendererVertex, texCoord), sizeof(RendererVertex));
    descriptor.AddAttribute(0, 3, VK_FORMAT_R8_SINT, offsetof(RendererVertex, texIndex), sizeof(RendererVertex));
    return descriptor;
  }
}