#include "copium/ecs/EntitySet.h"

namespace Copium
{
  bool EntitySet::Emplace(EntityID entity)
  {
    auto res = entitiesMap.emplace(entity, entitiesList.size());

    // Check if already exists
    if (!res.second)
      return false;

    entitiesList.push_back(entity);
    return true;
  }

  bool EntitySet::Erase(EntityID entity)
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

  size_t EntitySet::Find(EntityID entity)
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

  std::vector<EntityID>& EntitySet::GetList() { return entitiesList; }
  const std::vector<EntityID>& EntitySet::GetList() const { return entitiesList; }

  std::vector<EntityID>::iterator EntitySet::begin() { return entitiesList.begin(); }
  std::vector<EntityID>::iterator EntitySet::end() { return entitiesList.end(); }
}
