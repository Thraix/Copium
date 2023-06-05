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
      const float MAX_JUMP_TIME = 0.2;
      float force = 0.0f;
      if (Input::IsKeyPressed(CP_KEY_SPACE))
      {
        player.jumpTimer.Start();
        physics.velocity.y += 15;
      }
      else if (!Input::IsKeyDown(CP_KEY_SPACE))
      {
        if (physics.velocity.y > 0)
        {
          physics.force.y -= 12;
        }
      }
      if (Input::IsKeyDown(CP_KEY_A)) force -= 1.0f;
      if (Input::IsKeyDown(CP_KEY_D)) force += 1.0f;

      float magnitude = 75.0f;
      physics.force.x += force * magnitude;

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
