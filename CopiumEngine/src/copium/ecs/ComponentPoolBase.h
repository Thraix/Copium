#pragma once

#include <vector>

#include "copium/ecs/Config.h"
#include "copium/ecs/EntitySet.h"

namespace Copium
{
  class ComponentPoolBase
  {
  protected:
    EntitySet entities;

  public:
    virtual ~ComponentPoolBase() = default;

    virtual size_t Size() = 0;
    virtual bool Erase(EntityId entity) = 0;
    virtual void CommitUpdates() = 0;
    std::vector<EntityId>& GetEntities();
    const std::vector<EntityId>& GetEntities() const;
  };
}
