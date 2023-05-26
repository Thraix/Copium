#pragma once

#include "copium/ecs/System.h"
#include "copium/event/Input.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Copium
{
  class MouseFollowSystem : public System<MouseFollowC, TransformC>
  {
  private:
    glm::mat4* invPvMatrix;
  public:
    MouseFollowSystem(glm::mat4* invPvMatrix)
      : invPvMatrix{invPvMatrix}
    {}

    void RunEntity(Entity entity, MouseFollowC& mouseFollow, TransformC& transform)
    {
      transform.position = (*invPvMatrix) * glm::vec4{Input::GetMousePos().x, Input::GetMousePos().y, 0.0f, 1.0f};
      transform.position -= transform.size * glm::vec2{0.5f};
    }
  };
}
