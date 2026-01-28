#pragma once

#include <glm/glm.hpp>

#include "copium/pipeline/VertexDescriptor.h"

namespace Copium
{
  struct LineVertex
  {
    glm::vec3 pos;
    glm::vec3 color;

    static VertexDescriptor GetDescriptor();
  };
}
