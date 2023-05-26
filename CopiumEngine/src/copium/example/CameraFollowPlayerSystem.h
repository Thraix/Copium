#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"

namespace Copium
{
  class CameraFollowPlayerSystem : public System<PlayerC, TransformC>
  {
  public:
    void RunEntity(Entity entity, PlayerC& player, TransformC& transform) override
    {
      TransformC& cameraTransform = player.camera.GetComponent<TransformC>();
      glm::vec2 wantedPos = transform.position + transform.size * 0.5f;
      cameraTransform.position -= (cameraTransform.position - wantedPos) * 0.10f;
    }
  };
}
