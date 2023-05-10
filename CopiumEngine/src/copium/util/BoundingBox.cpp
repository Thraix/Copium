#include "copium/util/BoundingBox.h"

namespace Copium
{
  BoundingBox::BoundingBox()
    : l{0.0f}, b{0.0f}, r{0.0f}, t{0.0f}
  {}

  BoundingBox::BoundingBox(float all)
    : l{all}, b{all}, r{all}, t{all}
  {}

  BoundingBox::BoundingBox(float l, float b, float r, float t)
    : l{l}, b{b}, r{r}, t{t}
  {}

  BoundingBox::BoundingBox(glm::vec2 lb, glm::vec2 rt)
    : lb{lb}, rt{rt}
  {}

  glm::vec2 BoundingBox::GetSize() const 
  {
    return rt - lb;
  }
}
