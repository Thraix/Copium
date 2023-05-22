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

    virtual void RunEntity(Entity entity, Components&... components) = 0;
  };
}
