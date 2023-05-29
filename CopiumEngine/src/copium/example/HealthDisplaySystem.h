#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"


namespace Copium
{
  class HealthDisplaySystem : public System<HealthC, TransformC>
  {
    void RunEntity(Entity entity, HealthC& health, TransformC& transform) override
    {
      TransformC& foregroundTransform = health.foreground.GetComponent<TransformC>();
      foregroundTransform.position = transform.position + glm::vec2{0.0f, transform.size.y * 1.13};
      foregroundTransform.size = glm::vec2{std::clamp(health.current, 0, health.max) / (float)health.max, 0.2f};

      TransformC& backgroundTransform = health.background.GetComponent<TransformC>();
      backgroundTransform.position = transform.position + glm::vec2{0.0f, transform.size.y * 1.13};
    }
  };

}