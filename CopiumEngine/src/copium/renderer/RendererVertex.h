#pragma once

#include "copium/pipeline/VertexDescriptor.h"

#include <glm/glm.hpp>

namespace Copium
{
  struct RendererVertex
  {
    glm::vec2 position;
    glm::vec3 color;
    glm::vec2 texCoord;
    int8_t texIndex;

    static VertexDescriptor GetDescriptor();
  };
}
