#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"
#include "copium/event/Input.h"

namespace Copium
{
  class HealthChangeSystem : public System<HealthC>
  {

    void RunEntity(Entity entity, HealthC& health) override
    {
      if (Input::IsKeyPressed(CP_KEY_K))
        health.current++;
      if (Input::IsKeyPressed(CP_KEY_J))
        health.current--;
      health.current = std::clamp(health.current, 0, health.max);
    }
  };
}
