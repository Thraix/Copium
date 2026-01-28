#pragma once

#include <glm/glm.hpp>

#include "copium/pipeline/VertexDescriptor.h"

namespace Copium
{
  struct Vertex
  {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;

    static VertexDescriptor GetDescriptor();
  };
}
