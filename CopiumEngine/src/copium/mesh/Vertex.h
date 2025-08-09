#pragma once

#include "copium/pipeline/VertexDescriptor.h"

#include <glm/glm.hpp>

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

