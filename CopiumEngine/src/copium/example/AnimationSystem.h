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
        texture.texCoord1.x = (animation.sheetStartCoordX + animation.frame) / (float)animation.sheetSizeX;
        texture.texCoord2.x = (animation.sheetStartCoordX + animation.frame + 1) / (float)animation.sheetSizeX;
        texture.texCoord1.y = (animation.sheetStartCoordY) / (float)animation.sheetSizeY;
        texture.texCoord2.y = (animation.sheetStartCoordY + 1) / (float)animation.sheetSizeY;
      }
      else {
        texture.texCoord1.x = (animation.sheetStartCoordX) / (float)animation.sheetSizeX;
        texture.texCoord2.x = (animation.sheetStartCoordX + 1) / (float)animation.sheetSizeX;
        texture.texCoord1.y = (animation.sheetStartCoordY + animation.frame) / (float)animation.sheetSizeY;
        texture.texCoord2.y = (animation.sheetStartCoordY + animation.frame + 1) / (float)animation.sheetSizeY;
      }
    }
  };
}
