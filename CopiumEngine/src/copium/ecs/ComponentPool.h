#pragma once

#include "copium/ecs/Config.h"
#include "copium/ecs/EntitySet.h"
#include "copium/ecs/ComponentPoolBase.h"
#include "copium/ecs/ComponentListener.h"

#include <vector>

namespace Copium
{
  template <typename Component>
  class ComponentPool : public ComponentPoolBase
  {
    using Iterator = typename std::vector<Component>::iterator;
  private:
    std::vector<Component> components;
    ComponentListener<Component>* listener = nullptr;

  public:
    ComponentPool()
    {}

    ~ComponentPool() override
    {
      if(listener)
        delete listener;
    }

    ComponentPool(EntityId entity, const Component& component)
    {
      Emplace(entity, component);
    }

    Component& Emplace(EntityId entity, const Component& component)
    {
      components.push_back(component);
      entities.Emplace(entity);
      if(listener)
        listener->Added(entity, components.back());
      return components.back();
    }

    bool Erase(EntityId entity) override
    {
      size_t index = entities.Find(entity);
      if (!entities.Erase(entity))
        return false;
      if (listener)
      {
        auto it = components.begin() + index;
        Component component = *it;
        components.erase(it);
        listener->Removed(entity, component);
      }
      else
      {
        components.erase(components.begin() + index);
      }
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

    void SetComponentListener(ComponentListener<Component>* listener)
    {
      ComponentPool::listener = listener;
    }

    Component& operator[](size_t index)
    {
      CP_ASSERT(index < components.size(), "Index Out of Bound Exception");
      return components[index];
    }

    size_t Size() override
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
