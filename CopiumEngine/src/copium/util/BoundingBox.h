#pragma once

#include <glm/glm.hpp>

namespace Copium
{
  struct BoundingBox
  {
    float l;
    float b;
    float r;
    float t;
    BoundingBox();
    BoundingBox(float all);
    BoundingBox(float l, float b, float r, float t);
    BoundingBox(glm::vec2 lb, glm::vec2 rt);

    glm::vec2 GetSize() const;

    bool operator==(const BoundingBox& boundingBox) const;
    bool operator!=(const BoundingBox& boundingBox) const;

    glm::vec2 AsLb() const;
    glm::vec2 AsRt() const;
  };
}
