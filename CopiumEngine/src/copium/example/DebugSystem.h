#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"

#include <string>

namespace Copium
{
  class DebugSystem : public System<DebugC, TextC, TransformC>
  {
    BoundingBox* viewport;
  public:
    DebugSystem(BoundingBox* viewport)
      : viewport{viewport}
    {}

    void RunEntity(Entity entity, DebugC& debug, TextC& text, TransformC& transform) override
    {
      const PlayerC& player = debug.playerEntity.GetComponent<PlayerC>();
      const TransformC& playerTransform = debug.playerEntity.GetComponent<TransformC>();
      const PhysicsC& playerPhysics = debug.playerEntity.GetComponent<PhysicsC>();
      const glm::vec2& velocity = playerPhysics.velocity;

      text.text = "";
      text.text += String::Format("Position: (%.3f, %.3f)\n", playerTransform.position.x, playerTransform.position.y);
      text.text += String::Format("Velocity: (%.3f, %.3f)\n", velocity.x, velocity.y) ;
      text.text += String::Format("Grounded: %s", player.grounded ? "true" : "false");

      const Font& font = AssetManager::GetAsset<Font>(text.font);
      transform.position.y = viewport->GetSize().y - 10.0f - font.GetBaseHeight() * text.fontSize;
    }
  };
}