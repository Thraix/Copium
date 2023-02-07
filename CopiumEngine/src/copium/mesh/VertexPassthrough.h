#pragma once

#include "copium/pipeline/VertexDescriptor.h"

#include <glm/glm.hpp>

namespace Copium
{
  struct VertexPassthrough 
  {
    glm::vec2 texCoord;

    static VertexDescriptor GetDescriptor();
  };
}
