#pragma once

#include "copium/asset/AssetRef.h"
#include "copium/ecs/Entity.h"
#include "copium/util/Uuid.h"
#include "copium/util/BoundingBox.h"

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

  struct CameraC
  {
    BoundingBox projection;
    bool staticBoundingBox;
    bool uiCamera;
  };

  struct PhysicsC
  {
    float mass;

    glm::vec2 force{};
    glm::vec2 velocity{};
  };

  struct PlayerC
  {
    Entity camera;

    bool grounded = false;
  };

  struct HealthC
  {
    int current;
    int max;

    Entity background{};
    Entity foreground{};
  };

  struct StaticColliderC
  {
    bool resolveCollision;
  };

  struct DynamicColliderC
  {
    bool resolveCollision;
    glm::vec2 colliderOffset;
    glm::vec2 colliderSize;

    glm::vec2 oldPosition{};
  };


  struct DebugC
  {
    Entity playerEntity;
  };

  struct AnimationC
  {
    glm::ivec2 sheetCoord;
    glm::ivec2 sheetSize;
    int images;
    bool horizontal;
    float time;

    float timeElapsed = 0.0f;
    int frame = 0;
  };

  struct UuidC
  {
    Uuid uuid;
  };

  struct LevelGeneratorC
  {
    std::vector<Entity> entities;
  };

  struct NameC
  {
    std::string name;
  };

  struct RenderableC {};
  struct UiRenderableC {};
  struct SerializableC {};
  struct PickupC {};
  struct MouseFollowC {};
  struct FrameCountC {};
}