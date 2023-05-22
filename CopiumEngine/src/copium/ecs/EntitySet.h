#pragma once

#include "copium/ecs/Config.h"

#include <unordered_map>
#include <vector>

namespace Copium
{
  class EntitySet
  {
  private:
    std::vector<EntityId> entitiesList;
    std::unordered_map<EntityId, size_t> entitiesMap; // Maps the entity id to a component index
  public:
    bool Emplace(EntityId entity);
    bool Erase(EntityId entity);
    bool Pop();
    size_t Find(EntityId entity);
    size_t Size() const;
    std::vector<EntityId>& GetList();
    const std::vector<EntityId>& GetList() const;

    std::vector<EntityId>::iterator begin();
    std::vector<EntityId>::iterator end();
  };
}
