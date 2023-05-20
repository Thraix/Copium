#pragma once

#include "copium/asset/AssetRef.h"

#include <string>

#include <glm/glm.hpp>

namespace Copium
{
  struct TransformC
  {
    glm::vec2 position;
    glm::vec2 size;
  };

  struct ColorC
  {
    glm::vec3 color;
  };

  struct TextureC
  {
    AssetRef asset;
    glm::vec2 texCoord1;
    glm::vec2 texCoord2;
  };

  struct TextC
  {
    AssetRef font;
    std::string text;
    float fontSize;
  };

  struct MouseFollowC
  {};

  struct FrameCountC
  {};
}