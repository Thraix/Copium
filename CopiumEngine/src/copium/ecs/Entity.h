#pragma once

#include "copium/ecs/Config.h"
#include "copium/ecs/ECSManager.h"

namespace Copium
{
  class Entity
  {
    friend class ECSManager;

  private:
    ECSManager* manager;
    EntityID id;

  public:
    Entity();
    Entity(ECSManager* manager);
    Entity(ECSManager* manager, EntityID id);

    operator EntityID() const;
    void operator=(EntityID entityId);
    bool operator==(const Entity& entity);
    bool operator!=(const Entity& entity);
    operator bool() const;

    void Invalidate();
    void Destroy();
    void SetID(EntityID aId);
    EntityID GetID() const;
    ECSManager* GetManager() const;

    static Entity Create(ECSManager* manager);

    template <typename Component, typename... Args>
    inline Component& AddComponent(Args... args)
    {
      return manager->AddComponent<Component>(id, args...);
    }

    template <typename... Components>
    std::tuple<Components&...> AddComponents(Components&&... components)
    {
      return manager->AddComponents(id, components...);
    }

    template <typename Component>
    inline void RemoveComponent()
    {
      return manager->RemoveComponent<Component>(id);
    }

    template <typename... Components>
    inline void RemoveComponents()
    {
      return manager->RemoveComponents<Components...>(id);
    }

    template <typename Component>
    inline Component& GetComponent() const
    {
      return manager->GetComponent<Component>(id);
    }

    template <typename Component>
    inline bool HasComponent() const
    {
      return manager->HasComponent<Component>(id);
    }

    template <typename... Components>
    inline bool HasComponents() const
    {
      return manager->HasComponents<Components...>(id);
    }

    template <typename... Components>
    inline bool HasAnyComponent() const
    {
      return manager->HasAnyComponent<Components...>(id);
    }
  };
}
