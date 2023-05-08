#pragma once

#include <glm/glm.hpp>

namespace Copium
{
  struct Glyph
  {
    float advance;
    glm::vec2 pos1;
    glm::vec2 pos2;
    glm::vec2 texCoord1;
    glm::vec2 texCoord2;
  };
}
