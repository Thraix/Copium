#pragma once


#include "copium/ecs/ComponentPool.h"
#include "copium/ecs/Config.h"
#include "copium/ecs/Signal.h"
#include "copium/ecs/SystemPool.h"
#include "copium/util/Common.h"

#include <functional>
#include <map>
#include <typeindex>
#include <unordered_set>

namespace Copium
{
  class ECSManager final
  {
  private:
    std::unordered_set<EntityId> entities;
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

    template <typename SystemClass>
    void RemoveSystem()
    {
      systemPool->RemoveSystem(typeid(SystemClass));
    }

    void UpdateSystems();
    void UpdateSystems(const Signal& signal);

    EntityId CreateEntity();
    void DestroyEntity(EntityId entity);
    size_t GetEntityCount() const;
    bool ValidEntity(EntityId entity);
    void Each(std::function<void(EntityId)> function);

    template <typename Listener, typename... Args>
    void SetComponentListener(const Args&... args)
    {
      using Component = typename Listener::component_type;
      auto pool = GetComponentPool<Component>();
      Listener* listener = new Listener{args...};
      listener->manager = this;
      if (pool)
      {
        pool->SetComponentListener(listener);
      }
      else
      {
        ComponentPool<Component>* pool{new ComponentPool<Component>{}};
        componentPool.emplace(GetComponentId<Component>(), pool);
        pool->SetComponentListener(listener);
      }
    }

    template <typename... Components>
    std::tuple<Components&...> AddComponents(EntityId entity, Components&&... components)
    {
      return std::forward_as_tuple(AddComponent(entity, Components(components))...);
    }

    template <typename Component, typename... Args>
    Component& AddComponent(EntityId entity, Args&&... args)
    {
      return AddComponent(entity, Component{args...});
    }

    template <typename Component>
    Component& AddComponent(EntityId entity, const Component& component)
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
    void RemoveComponent(EntityId entity)
    {
      auto pool = GetComponentPoolAssure<Component>();
      CP_ASSERT(pool->Erase(entity), "Entity did not contain component (entity=%u, Component=%s)", entity, typeid(Component).name());
    }

    template <typename... Components>
    void RemoveComponents(EntityId entity)
    {
      (RemoveComponent<Components>(entity), ...);
    }

    template <typename Component>
    Component& GetComponent(EntityId entity)
    {
      auto pool = GetComponentPoolAssure<Component>();
      Component* component = pool->FindComponent(entity);
      CP_ASSERT(component, "Entity did not contain component (entity=%u, Component=%s)", entity, typeid(Component).name());
      return *component;
    }

    template <typename Component>
    bool HasComponent(EntityId entity)
    {
      auto pool = GetComponentPool<Component>();
      if (pool)
        return pool->Find(entity) != pool->Size();
      return false;
    }

    template <typename... Components>
    bool HasComponents(EntityId entity)
    {
      return (HasComponent<Components>(entity) && ...);
    }

    template <typename... Components>
    bool HasAnyComponent(EntityId entity)
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
    void Each(std::function<void(EntityId, Component&)> function)
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
    EntityId Find(Func function)
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
    EntityId Find(std::function<bool(EntityId, Component&)> function)
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

    template <typename Component, typename... Components>
    EntityId Find()
    {
      return Find<Component, Components...>([] (EntityId, const Component& component, const Components&... components) { return true; });
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
