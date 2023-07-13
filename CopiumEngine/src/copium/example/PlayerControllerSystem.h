#pragma once

#include "copium/ecs/System.h"
#include "copium/event/Input.h"
#include "copium/example/Components.h"
#include "copium/example/CollideSignal.h"

namespace Copium
{
  class PlayerControllerSystem : public System<PlayerC, PhysicsC, AnimationC>
  {
  public:
    void RunEntity(Entity entity, PlayerC& player, PhysicsC& physics, AnimationC& animation) override
    {
      if (physics.velocity.y != 0.0f)
        player.grounded = false;
      const float MAX_JUMP_TIME = 0.2;
      float force = 0.0f;
      if (Input::IsKeyPressed(CP_KEY_SPACE) && player.grounded)
      {
        physics.velocity.y = 15;
      }
      else if (!Input::IsKeyDown(CP_KEY_SPACE))
      {
        if (physics.velocity.y > 0)
        {
          physics.force.y -= 12;
        }
      }
      animation.time = 0.5;
      if (Input::IsKeyDown(CP_KEY_A)) {
        force -= 1.0f;
        animation.sheetCoord.y = 0;
        animation.time = 0.2;
      }

      if (Input::IsKeyDown(CP_KEY_D)) {
        force += 1.0f;
        animation.sheetCoord.y = 1;
        animation.time = 0.2;
      }

      if (force == 0.0f && animation.sheetCoord.y == 1)
        animation.sheetCoord.y = 3;
      if (force == 0.0f && animation.sheetCoord.y == 0)
        animation.sheetCoord.y = 2;

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

    void Run(const Signal& signal) override
    {
      if (signal.GetId() == CollideSignal::GetIdStatic())
      {
        const CollideSignal& collideSignal = static_cast<const CollideSignal&>(signal);
        if (collideSignal.GetFirst().HasComponent<PlayerC>() && collideSignal.WasResolved())
        {
          if (collideSignal.GetYDir() == -1)
            collideSignal.GetFirst().GetComponent<PlayerC>().grounded = true;
        }
      }
    }
  };
}
