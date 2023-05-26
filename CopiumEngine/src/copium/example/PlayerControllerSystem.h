#pragma once

#include "copium/ecs/System.h"
#include "copium/event/Input.h"
#include "copium/example/Components.h"

namespace Copium
{
  class PlayerControllerSystem : public System<PlayerC, PhysicsC>
  {
  public:
    void RunEntity(Entity entity, PlayerC& player, PhysicsC& physics) override
    {
      float magnitude = 200.0f;
      glm::vec2 force{0.0f};
      if (Input::IsKeyDown(CP_KEY_W)) force.y += 1.0f;
      if (Input::IsKeyDown(CP_KEY_S)) force.y -= 1.0f;
      if (Input::IsKeyDown(CP_KEY_A)) force.x -= 1.0f;
      if (Input::IsKeyDown(CP_KEY_D)) force.x += 1.0f;

      glm::normalize(force);
      physics.force += force * magnitude;
    }
  };
}
