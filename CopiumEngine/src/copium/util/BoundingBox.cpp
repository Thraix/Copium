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
    : l{lb.x}, b{lb.y}, r{rt.x}, t{rt.y}
  {}

  glm::vec2 BoundingBox::GetSize() const
  {
    return glm::abs(AsRt() - AsLb());
  }

  bool BoundingBox::operator==(const BoundingBox& boundingBox) const
  {
    return l == boundingBox.l && b == boundingBox.b && r == boundingBox.r && t == boundingBox.t;
  }

  bool BoundingBox::operator!=(const BoundingBox& boundingBox) const
  {
    return !(*this == boundingBox);
  }

  glm::vec2 BoundingBox::AsLb() const
  {
    return glm::vec2{l, b};
  }

  glm::vec2 BoundingBox::AsRt() const
  {
    return glm::vec2{r, t};
  }
}
