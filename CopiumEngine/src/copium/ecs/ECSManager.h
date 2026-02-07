#pragma once

#include <functional>
#include <map>
#include <set>
#include <typeindex>
#include <unordered_set>

#include "copium/ecs/ComponentPool.h"
#include "copium/ecs/Config.h"
#include "copium/ecs/Signal.h"
#include "copium/ecs/SystemPool.h"
#include "copium/util/Common.h"
#include "copium/util/GenericType.h"
#include "copium/util/Uuid.h"

namespace Copium
{
  class ECSManager final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(ECSManager);

  private:
    std::unordered_set<EntityId> entities;
    std::map<std::type_index, ComponentPoolBase*> componentPools;

    std::map<Uuid, std::unique_ptr<SystemPool>> systemPools;
    EntityId currentEntityId = 1;
    std::set<EntityId> destroyedEntityIds;

    std::map<std::type_index, GenericType> globalDatas;

  public:
    static std::vector<EntityId> emptyEntities;

    ECSManager();
    ~ECSManager();

    template <typename SystemClass, typename... Args>
    SystemOrderer& AddSystem(const Uuid& systemPoolId)
    {
      auto it = systemPools.find(systemPoolId);
      if (it == systemPools.end())
        it = systemPools.emplace(systemPoolId, std::make_unique<SystemPool>(this)).first;

      return it->second->AddSystem(typeid(SystemClass), new SystemClass{});
    }

    template <typename SystemClass>
    void RemoveSystem(const Uuid& systemPoolId)
    {
      auto it = systemPools.find(systemPoolId);
      CP_ASSERT(it != systemPools.end(), "SystemPool doesn't exist with Uuid=%s", systemPoolId.ToString().c_str());

      it->second->RemoveSystem(typeid(SystemClass));

      if (it->second->IsEmpty())
        systemPools.erase(it);
    }

    void CommitEntityUpdates();

    void UpdateSystems(const Uuid& systemPoolId);

    template <typename S, typename... Args>
    void SendSignal(const Args&... args)
    {
      Signal::ValidateSignal<S>();
      for (auto& systemPool : systemPools)
      {
        S* newSignal = new S{args...};
        newSignal->uuid = S::UUID;
        systemPool.second->SendSignal(newSignal);
      }
    }

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
        componentPools.emplace(GetComponentId<Component>(), pool);
        pool->SetComponentListener(listener);
      }
    }

    template <typename... Components>
    void AddComponents(EntityId entity, Components&&... components)
    {
      (AddComponent(entity, components), ...);
    }

    template <typename Component, typename... Args>
    void AddComponent(EntityId entity, Args&&... args)
    {
      AddComponent(entity, Component{args...});
    }

    template <typename Component>
    void AddComponent(EntityId entity, const Component& component)
    {
      auto pool = GetComponentPool<Component>();

      if (pool)
      {
        pool->Emplace(entity, component);
      }
      else
      {
        ComponentPool<Component>* pool{new ComponentPool{entity, component}};
        auto ret = componentPools.emplace(GetComponentId<Component>(), pool);
      }
    }

    template <typename Component>
    void RemoveComponent(EntityId entity)
    {
      auto pool = GetComponentPoolAssure<Component>();
      pool->Erase(entity);
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
      CP_ASSERT(
        component, "Entity did not contain component (entity=%u, Component=%s)", entity, typeid(Component).name());
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
      return Find<Component, Components...>([](EntityId, const Component& component, const Components&... components)
                                            { return true; });
    }

    template <typename T>
    std::type_index GetComponentId()
    {
      return std::type_index(typeid(T));
    }

    template <typename T, typename... Args>
    void AddGlobalData(const Args&... args)
    {
      auto it = globalDatas.find(typeid(T));
      CP_ASSERT(!HasGlobalData<T>(), "Global with typeid=%s already exists. Do nothing", typeid(T).name());

      globalDatas.emplace(typeid(T), GenericType::Create<T>(args...));
    }

    template <typename T>
    T& GetGlobalData()
    {
      auto it = globalDatas.find(typeid(T));
      CP_ASSERT(it != globalDatas.end(), "Global with typeid=%s doesn't exist");

      return it->second.Get<T>();
    }

    template <typename T>
    bool HasGlobalData()
    {
      return globalDatas.find(typeid(T)) != globalDatas.end();
    }

    template <typename T>
    void RemoveGlobalData()
    {
      auto it = globalDatas.find(typeid(T));
      CP_ASSERT("Global with typeid=%s doesn't exist. Do nothing", typeid(T).name());
      globalDatas.erase(it);
    }

    template <typename Component>
    ComponentPool<std::remove_const_t<Component>>* GetComponentPool()
    {
      auto it = componentPools.find(GetComponentId<Component>());
      return it == componentPools.end() ? nullptr
                                        : static_cast<ComponentPool<std::remove_const_t<Component>>*>(it->second);
    }

    template <typename Component>
    ComponentPool<std::remove_const_t<Component>>* GetComponentPoolAssure()
    {
      auto it = componentPools.find(GetComponentId<Component>());
      CP_ASSERT(it != componentPools.end(),
                "Component has not been added to an entity (Component=%s)",
                typeid(Component).name());
      return static_cast<ComponentPool<std::remove_const_t<Component>>*>(it->second);
    }
  };
}
