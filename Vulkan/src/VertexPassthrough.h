#pragma once

#include <glm/glm.hpp>
#include "VertexDescriptor.h"

namespace Copium
{
  struct VertexPassthrough 
  {
    glm::vec2 texCoord;

    static VertexDescriptor GetDescriptor();
  };
}
