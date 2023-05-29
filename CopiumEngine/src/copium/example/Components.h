#pragma once

#include "copium/asset/AssetRef.h"
#include "copium/ecs/Entity.h"

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

  struct CameraC
  {
    BoundingBox projection;
    bool staticBoundingBox;
  };

  struct PhysicsC
  {
    float mass;
    glm::vec2 force;
    glm::vec2 velocity;
  };

  struct PlayerC
  {
    Entity camera;
  };

  struct HealthC
  {
    int current;
    int max;

    Entity background;
    Entity foreground;
  };
}