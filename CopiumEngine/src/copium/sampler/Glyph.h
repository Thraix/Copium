#pragma once

#include "copium/util/BoundingBox.h"

namespace Copium
{
  struct Glyph
  {
    float advance;
    BoundingBox boundingBox;
    BoundingBox texCoordBoundingBox;
  };
}
