#pragma once

#include <glm/glm.hpp>

#include "copium/pipeline/VertexDescriptor.h"

namespace Copium
{
  struct RendererVertex
  {
    static const int TYPE_QUAD = 0;
    static const int TYPE_TEXT = 1;
    glm::vec2 position;
    glm::vec3 color;
    glm::vec2 texCoord;
    int8_t texIndex;
    int8_t type = TYPE_QUAD;  // TODO: Should maybe have different Renderers for quad vs text instead of a flag

    static VertexDescriptor GetDescriptor();
  };
}
