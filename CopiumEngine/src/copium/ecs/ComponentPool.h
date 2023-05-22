#pragma once

#include "copium/ecs/Config.h"
#include "copium/ecs/EntitySet.h"
#include "copium/ecs/ComponentPoolBase.h"

#include <vector>

namespace Copium
{
  template <typename Component>
  class ComponentPool : public ComponentPoolBase
  {
    using Iterator = typename std::vector<Component>::iterator;
  private:
    std::vector<Component> components;

  public:
    ComponentPool(EntityId entity, const Component& component)
    {
      Emplace(entity, component);
    }

    Component& Emplace(EntityId entity, const Component& component)
    {
      components.push_back(component);
      entities.Emplace(entity);
      return components.back();
    }

    void Pop()
    {
      components.pop_back();
      entities.Pop();
    }

    bool Erase(EntityId entity)
    {
      size_t index = entities.Find(entity);
      if (!entities.Erase(entity))
        return false;
      components.erase(components.begin() + index);
      return true;
    }

    Component& At(size_t index)
    {
      return operator[](index);
    }

    size_t Find(EntityId entity)
    {
      return entities.Find(entity);
    }

    Component* FindComponent(EntityId entity)
    {
      size_t index = Find(entity);
      if (index < Size())
        return &components[index];
      return nullptr;
    }

    Component& operator[](size_t index)
    {
      CP_ASSERT(index < components.size(), "Index Out of Bound Exception");
      return components[index];
    }

    size_t Size()
    {
      return components.size();
    }

    Iterator Back()
    {
      return components.back();
    }

    Iterator begin()
    {
      return components.begin();
    }

    Iterator end()
    {
      return components.end();
    }
  };
}
