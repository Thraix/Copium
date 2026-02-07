#include "copium/ecs/ECSManager.h"

#include "copium/util/Common.h"

namespace Copium
{
  std::vector<EntityId> ECSManager::emptyEntities = {};

  ECSManager::ECSManager()
  {
  }

  ECSManager::~ECSManager()
  {
    for (auto&& components : componentPools)
    {
      delete components.second;
    }
    componentPools.clear();
  }

  void ECSManager::CommitEntityUpdates()
  {
    for (auto& componentPool : componentPools)
    {
      componentPool.second->CommitUpdates();
    }
  }

  void ECSManager::UpdateSystems(const Uuid& systemPoolId)
  {
    auto it = systemPools.find(systemPoolId);
    CP_ASSERT(it != systemPools.end(), "SystemPool doesn't exist with Uuid=%s", systemPoolId.ToString().c_str());
    it->second->CommitSignals();
    CommitEntityUpdates();
    it->second->CommitUpdates();
    it->second->Update();
  }

  size_t ECSManager::GetEntityCount() const
  {
    return entities.size();
  }

  EntityId ECSManager::CreateEntity()
  {
    if (!destroyedEntityIds.empty())
    {
      EntityId newId = *destroyedEntityIds.begin();
      destroyedEntityIds.erase(destroyedEntityIds.begin());
      return newId;
    }

    CP_ASSERT(currentEntityId != MAX_NUM_ENTITIES, "No more entities available");
    entities.emplace(currentEntityId);
    currentEntityId++;
    return currentEntityId - 1;
  }

  void ECSManager::DestroyEntity(EntityId entity)
  {
    auto it = entities.find(entity);
    CP_ASSERT(it != entities.end(), "Entity does not exist in ECSManager (entity=%u)", entity);
    if (entity == currentEntityId - 1)
    {
      currentEntityId--;
      while (!destroyedEntityIds.empty() && *destroyedEntityIds.rbegin() == currentEntityId - 1)
      {
        destroyedEntityIds.erase(std::prev(destroyedEntityIds.end()));
      }
    }
    else
    {
      destroyedEntityIds.emplace(entity);
    }

    entities.erase(it);
    for (auto&& pool : componentPools)
    {
      pool.second->Erase(entity);
    }
  }

  bool ECSManager::ValidEntity(EntityId entity)
  {
    return entities.find(entity) != entities.end();
  }

  void ECSManager::Each(std::function<void(EntityId)> function)
  {
    for (auto e : entities)
      function(e);
  }
}
