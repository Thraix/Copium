#pragma once

#include "copium/pipeline/VertexDescriptor.h"

#include <glm/glm.hpp>

namespace Copium
{
  struct LineVertex
  {
    glm::vec3 pos;
    glm::vec3 color;

    static VertexDescriptor GetDescriptor();
  };
}
