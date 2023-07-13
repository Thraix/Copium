#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"

namespace Copium
{
  class AnimationSystem : public System<AnimationC, TextureC>
  {
    void RunEntity(Entity entity, AnimationC& animation, TextureC& texture) override
    {
      animation.timeElapsed += 1 / 165.0f; // TODO: Update to real Timestep
      if (animation.timeElapsed >= animation.time)
      {
        animation.timeElapsed -= animation.time;
        animation.frame = (animation.frame + 1) % animation.images;
      }
      if (animation.horizontal) {
        texture.texCoord1.x = (animation.sheetCoord.x + animation.frame) / (float)animation.sheetSize.x;
        texture.texCoord2.x = (animation.sheetCoord.x + animation.frame + 1) / (float)animation.sheetSize.x;
        texture.texCoord1.y = (animation.sheetCoord.y) / (float)animation.sheetSize.y;
        texture.texCoord2.y = (animation.sheetCoord.y + 1) / (float)animation.sheetSize.y;
      }
      else {
        texture.texCoord1.x = (animation.sheetCoord.x) / (float)animation.sheetSize.x;
        texture.texCoord2.x = (animation.sheetCoord.x + 1) / (float)animation.sheetSize.x;
        texture.texCoord1.y = (animation.sheetCoord.y + animation.frame) / (float)animation.sheetSize.y;
        texture.texCoord2.y = (animation.sheetCoord.y + animation.frame + 1) / (float)animation.sheetSize.y;
      }
    }
  };
}
