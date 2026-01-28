#pragma once

#include <glm/glm.hpp>

#include "copium/pipeline/VertexDescriptor.h"

namespace Copium
{
  struct VertexPassthrough
  {
    glm::vec2 texCoord;

    static VertexDescriptor GetDescriptor();
  };
}
