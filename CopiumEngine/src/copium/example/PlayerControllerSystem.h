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

      if (force.x != 0 || force.y != 0)
      {
        force = glm::normalize(force);
        physics.force += force * magnitude;
      }

      if (Input::IsKeyPressed(CP_KEY_H))
      {
        if (entity.HasComponent<HealthC>())
          entity.RemoveComponent<HealthC>();
        else
          entity.AddComponent<HealthC>(8, 10);
      }
    }
  };
}
