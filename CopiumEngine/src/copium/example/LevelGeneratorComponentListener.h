#pragma once

#include "copium/asset/AssetManager.h"
#include "copium/ecs/ComponentListener.h"
#include "copium/example/Components.h"

namespace Copium
{
  class LevelGeneratorComponentListener : public ComponentListener<LevelGeneratorC>
  {
    void Added(EntityId entityId, LevelGeneratorC& levelGenerator) override
    {
      CP_ASSERT(levelGenerator.entities.empty(), "LevelGenerator has already generated the level");

      for (int y = 0; y < 20; y++)
      {
        {
          Entity entity = Entity::Create(manager);
          entity.AddComponent<TransformC>(glm::vec2{-10.0f, -10.0f + y * 1.0}, glm::vec2{1.0f, 1.0f});
          if(y == 0 || y == 19)
            entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.0f, 0.0f}, glm::vec2{0.25f, 1.0f});
          else
            entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.75f, 0.0f}, glm::vec2{1.0f, 1.0f});
          entity.AddComponent<StaticColliderC>(true);
          entity.AddComponent<RenderableC>();
          levelGenerator.entities.emplace_back(entity);
        }
        {
          Entity entity = Entity::Create(manager);
          entity.AddComponent<TransformC>(glm::vec2{10.0f, -10.0f + y * 1.0}, glm::vec2{1.0f, 1.0f});
          if(y == 0 || y == 19)
            entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.5f, 0.0f}, glm::vec2{0.75f, 1.0f});
          else
            entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.75f, 0.0f}, glm::vec2{1.0f, 1.0f});
          entity.AddComponent<StaticColliderC>(true);
          entity.AddComponent<RenderableC>();
          levelGenerator.entities.emplace_back(entity);
        }
      }
      for (int x = 1; x < 20; x++)
      {
        {
          Entity entity = Entity::Create(manager);
          entity.AddComponent<TransformC>(glm::vec2{-10.0f + x * 1.0, -10.0f}, glm::vec2{1.0f, 1.0f});
          entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.25f, 0.0f}, glm::vec2{0.5f, 1.0f});
          entity.AddComponent<StaticColliderC>(true);
          entity.AddComponent<RenderableC>();
          levelGenerator.entities.emplace_back(entity);
        }
        {
          Entity entity = Entity::Create(manager);
          entity.AddComponent<TransformC>(glm::vec2{-10.0f + x * 1.0, 10.0f}, glm::vec2{1.0f, 1.0f});
          entity.AddComponent<TextureC>(AssetRef{AssetManager::LoadAsset("stone.meta")}, glm::vec2{0.25f, 0.0f}, glm::vec2{0.5f, 1.0f});
          entity.AddComponent<StaticColliderC>(true);
          entity.AddComponent<RenderableC>();
          levelGenerator.entities.emplace_back(entity);
        }
      }
      for (int y = 0; y < 10; y++)
      {
        for (int x = 0; x < 10; x++)
        {
          Entity entity = Entity::Create(manager);
          entity.AddComponent<TransformC>(glm::vec2{-10.0f + x * 1.6f + 0.4f, -10.0f + y * 1.6 + 0.4f}, glm::vec2{0.8f, 0.8f});
          entity.AddComponent<ColorC>(glm::vec3{x * 0.1f, y * 0.1f, 1.0f});
          entity.AddComponent<StaticColliderC>(false);
          entity.AddComponent<PickupC>();
          entity.AddComponent<RenderableC>();
          levelGenerator.entities.emplace_back(entity);
        }
      }
    }

    void Removed(EntityId entityId, LevelGeneratorC& levelGenerator) override
    {
      for (auto& entity : levelGenerator.entities)
      {
        entity.Destroy();
      }
    }
  };
}