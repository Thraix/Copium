#pragma once

#include "copium/ecs/ComponentListener.h"
#include "copium/example/Components.h"
#include "copium/util/Common.h"

namespace Copium
{
  class HealthComponentListener : public ComponentListener<HealthC>
  {
    void Added(EntityId entityId, HealthC& health) override
    {
      CP_ASSERT(!health.background, "Health already has background entity assigned");
      CP_ASSERT(!health.foreground, "Health already has foreground entity assigned");

      health.background = Entity::Create(manager);
      health.background.AddComponent<TransformC>(glm::vec2{0.0f, 0.0f}, glm::vec2{1.0f, 0.2f});
      health.background.AddComponent<ColorC>(glm::vec3{0.9f, 0.2f, 0.2f});

      health.foreground = Entity::Create(manager);
      health.foreground.AddComponent<TransformC>(glm::vec2{0.0f, 0.0f}, glm::vec2{std::clamp(health.current, 0, health.max) / (float)health.max, 0.2f});
      health.foreground.AddComponent<ColorC>(glm::vec3{0.2f, 0.9f, 0.2f});
    }

    void Removed(EntityId entityId, HealthC& health) override
    {
      CP_ASSERT(health.background, "Health already removed background entity");
      CP_ASSERT(health.foreground, "Health already removed foreground entity");

      health.background.Destroy();
      health.foreground.Destroy();
    }
  };

}
