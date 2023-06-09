#include "copium/ecs/ECSManager.h"

#include "copium/util/Common.h"

namespace Copium
{
  ECSManager::ECSManager()
    : systemPool{std::make_unique<SystemPool>(this)}
  {}

  ECSManager::~ECSManager()
  {
    for (auto&& components : componentPool)
    {
      delete components.second;
    }
    componentPool.clear();
  }

  void ECSManager::UpdateSystems()
  {
    systemPool->Update();
  }

  void ECSManager::UpdateSystems(const Signal& signal)
  {
    // TODO: Maybe we want a different pool for Signal based Systems for performance reasons?
    // Maybe even a pool for each type of Signal?
    systemPool->Update(signal);
  }

  size_t ECSManager::GetEntityCount() const
  {
    return entities.size();
  }

  EntityId ECSManager::CreateEntity()
  {
    CP_ASSERT(currentEntityId != MAX_NUM_ENTITIES, "No more entities available");
    entities.emplace(currentEntityId);
    currentEntityId++;
    return currentEntityId - 1;
  }

  void ECSManager::DestroyEntity(EntityId entity)
  {
    auto it = entities.find(entity);
    CP_ASSERT(it != entities.end(), "Entity does not exist in ECSManager (entity=%u)", entity);
    entities.erase(it);
    for (auto&& pool : componentPool)
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
