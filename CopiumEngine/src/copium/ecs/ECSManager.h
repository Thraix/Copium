#pragma once


#include "copium/ecs/ComponentPool.h"
#include "copium/ecs/Config.h"
#include "copium/ecs/SystemPool.h"
#include "copium/util/Common.h"

#include <functional>
#include <map>
#include <typeindex>
#include <unordered_set>
#include <vector>

namespace Copium
{
  class ECSManager final
  {
  private:
    std::unordered_set<EntityID> entities;
    std::map<std::type_index, ComponentPoolBase*> componentPool;

    std::unique_ptr<SystemPool> systemPool;
    int currentEntityId = 1;
  public:
    ECSManager();
    ~ECSManager();

    template <typename SystemClass, typename... Args>
    SystemOrderer AddSystem(const Args&... args)
    {
      return systemPool->AddSystem(typeid(SystemClass), new SystemClass{args...});
    }

    void UpdateSystems();

    EntityID CreateEntity();
    void DestroyEntity(EntityID entity);
    size_t GetEntityCount() const;
    bool ValidEntity(EntityID entity);
    void Each(std::function<void(EntityID)> function);

    template <typename... Components>
    std::tuple<Components&...> AddComponents(EntityID entity, Components&&... components)
    {
      return std::forward_as_tuple(AddComponent(entity, Components(components))...);
    }

    template <typename Component, typename... Args>
    Component& AddComponent(EntityID entity, Args&&... args)
    {
      return AddComponent(entity, Component{args...});
    }

    template <typename Component>
    Component& AddComponent(EntityID entity, const Component& component)
    {
      auto pool = GetComponentPool<Component>();

      if (pool)
      {
        CP_ASSERT(!HasComponent<Component>(entity), "Component already exists in entity (entity=%u, Component=%s)", entity, typeid(Component).name());
        return pool->Emplace(entity, component);
      }
      else
      {
        ComponentPool<Component>* pool{new ComponentPool{entity, component}};
        auto ret = componentPool.emplace(GetComponentId<Component>(), pool);
        return pool->At(0);
      }
    }

    template <typename Component>
    void RemoveComponent(EntityID entity)
    {
      auto pool = GetComponentPoolAssure<Component>();
      CP_ASSERT(pool->Erase(entity), "Entity did not contain component (entity=%u, Component=%s)", entity, typeid(Component).name());
    }

    template <typename... Components>
    void RemoveComponents(EntityID entity)
    {
      (RemoveComponent<Components>(entity), ...);
    }

    template <typename Component>
    Component& GetComponent(EntityID entity)
    {
      auto pool = GetComponentPoolAssure<Component>();
      Component* component = pool->FindComponent(entity);
      CP_ASSERT(component, "Entity did not contain component (entity=%u, Component=%s)", entity, typeid(Component).name());
      return *component;
    }

    template <typename Component>
    bool HasComponent(EntityID entity)
    {
      auto pool = GetComponentPool<Component>();
      if (pool)
        return pool->Find(entity) != pool->Size();
      return false;
    }

    template <typename... Components>
    bool HasComponents(EntityID entity)
    {
      return (HasComponent<Components>(entity) && ...);
    }

    template <typename... Components>
    bool HasAnyComponent(EntityID entity)
    {
      return (HasComponent<Components>(entity) || ...);
    }

    template <typename Component, typename... Components, typename Func>
    void Each(Func function)
    {
      auto pool = GetComponentPool<Component>();
      if (pool)
      {
        size_t i = 0;
        for (auto entity : pool->GetEntities())
        {
          if (HasComponents<Components...>(entity))
          {
            std::apply(function, std::forward_as_tuple(entity, pool->At(i), GetComponent<Components>(entity)...));
          }
          i++;
        }
      }
    }

    template <typename Component>
    void Each(std::function<void(EntityID, Component&)> function)
    {
      auto pool = GetComponentPool<Component>();
      if (pool)
      {
        size_t i = 0;
        for (auto e : pool->GetEntities())
        {
          function(e, pool->At(i));
          i++;
        }
      }
    }

    template <typename Component, typename... Components, typename Func>
    EntityID Find(Func function)
    {
      auto pool = GetComponentPool<Component>();
      if (pool)
      {
        size_t i = 0;
        for (auto entity : pool->GetEntities())
        {
          if (HasComponents<Components...>(entity))
          {
            if (std::apply(function, std::forward_as_tuple(entity, pool->At(i), GetComponent<Components>(entity)...)))
              return entity;
          }
          i++;
        }
      }
      return 0;
    }

    template <typename Component>
    EntityID Find(std::function<bool(EntityID, Component&)> function)
    {
      auto pool = GetComponentPool<Component>();
      if (pool)
      {
        size_t i = 0;
        for (auto e : pool->GetEntities())
        {
          if (function(e, pool->At(i)))
            return e;
          i++;
        }
      }
      return 0;
    }

    template <typename T>
    std::type_index GetComponentId()
    {
      return std::type_index(typeid(T));
    }

  private:
    template <typename Component>
    ComponentPool<Component>* GetComponentPool()
    {
      auto it = componentPool.find(GetComponentId<Component>());
      return it == componentPool.end() ? nullptr : static_cast<ComponentPool<Component>*>(it->second);
    }

    template <typename Component>
    ComponentPool<Component>* GetComponentPoolAssure()
    {
      auto it = componentPool.find(GetComponentId<Component>());
      CP_ASSERT(it != componentPool.end(), "Component has not been added to an entity (Component=%s)", typeid(Component).name());
      return static_cast<ComponentPool<Component>*>(it->second);
    }
  };
}
