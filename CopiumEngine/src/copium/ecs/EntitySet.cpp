#include "copium/ecs/EntitySet.h"

namespace Copium
{
  bool EntitySet::Emplace(EntityId entity)
  {
    auto res = entitiesMap.emplace(entity, entitiesList.size());

    // Check if already exists
    if (!res.second)
      return false;

    entitiesList.push_back(entity);
    return true;
  }

  bool EntitySet::Erase(EntityId entity)
  {
    auto it = entitiesMap.find(entity);
    if (it == entitiesMap.end())
      return false;
    size_t componentPos = it->second;
    entitiesList.erase(entitiesList.begin() + it->second);
    entitiesMap.erase(it);

    for (auto&& entityPos : entitiesMap)
    {
      if (entityPos.second > componentPos)
      {
        entityPos.second--;
      }
    }
    return true;
  }

  bool EntitySet::Pop()
  {
    if (entitiesList.size() == 0)
      return false;
    entitiesMap.erase(entitiesMap.find(entitiesList.back()));
    entitiesList.pop_back();
    return true;
  }

  size_t EntitySet::Find(EntityId entity)
  {
    auto it = entitiesMap.find(entity);
    if (it == entitiesMap.end())
      return entitiesList.size();
    return it->second;
  }

  size_t EntitySet::Size() const
  {
    return entitiesList.size();
  }

  std::vector<EntityId>& EntitySet::GetList()
  {
    return entitiesList;
  }

  const std::vector<EntityId>& EntitySet::GetList() const
  {
    return entitiesList;
  }

  std::vector<EntityId>::iterator EntitySet::begin()
  {
    return entitiesList.begin();
  }

  std::vector<EntityId>::iterator EntitySet::end()
  {
    return entitiesList.end();
  }
}
