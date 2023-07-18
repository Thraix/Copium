#pragma once

#include "copium/ecs/ECSManager.h"
#include "copium/ecs/SystemBase.h"
#include "copium/ecs/Entity.h"

namespace Copium
{
  template <typename... Components>
  class System : public SystemBase
  {
  public:
    void Run() override
    {
      manager->Each<Components...>([&](EntityId entityId, Components&... components) { RunEntity(Entity{manager, entityId}, components...); });
    }

    void Run(const Signal& signal) override
    {
      manager->Each<Components...>([&](EntityId entityId, Components&... components) { RunEntity(signal, Entity{manager, entityId}, components...); });
    }

    virtual void RunEntity(Entity entity, Components&... components) {};
    virtual void RunEntity(const Signal& signal, Entity entity, Components&... components) {};


    // TODO: Not sure if this is the way entities should be validated
    std::set<EntityId> loggedEntities;
    template <typename... Components>
    bool ValidateEntity(Entity entity)
    {
      if (entity && entity.HasComponents<Components...>())
      {
        loggedEntities.erase(entity);
        return true;
      }
      if (loggedEntities.find(entity) == loggedEntities.end())
        CP_WARN("Invalid Entity");
      loggedEntities.emplace(entity);
      return false;
    }
  };
}
