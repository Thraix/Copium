#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"

namespace Copium
{
  class PhysicsSystem : public System<PhysicsC, TransformC>
  {
  public:
    void RunEntity(Entity entity, PhysicsC& physics, TransformC& transform) override
    {
      float timespan = 1 / 165.0f; // My main monitor refresh rate, should be based on the frame rate
      physics.velocity += physics.force / physics.mass * timespan;
      physics.velocity *= 0.7; // friction
      transform.position += physics.velocity * timespan;
      physics.force = glm::vec2{0.0f};
    }
  };
}
