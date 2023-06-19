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
      foregroundTransform.size = glm::vec2{0.5f * std::clamp(health.current, 0, health.max) / (float)health.max, 0.05f};
      foregroundTransform.position = transform.position + glm::vec2{0.0f, transform.size.y * 0.73} + glm::vec2{transform.size.x * 0.5f - 0.25f, 0.0f};

      TransformC& backgroundTransform = health.background.GetComponent<TransformC>();
      backgroundTransform.position = transform.position + glm::vec2{0.0f, transform.size.y * 0.73} + glm::vec2{transform.size.x * 0.5f - backgroundTransform.size.x * 0.5f, 0.0f};
    }
  };

}