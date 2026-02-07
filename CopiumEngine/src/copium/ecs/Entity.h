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
    EntityId id;

  public:
    Entity();
    Entity(ECSManager* manager);
    Entity(ECSManager* manager, EntityId id);

    operator EntityId() const;
    void operator=(EntityId entityId);
    bool operator==(const Entity& entity);
    bool operator!=(const Entity& entity);
    operator bool() const;

    bool IsValid() const;
    void Invalidate();
    void Destroy();
    void SetId(EntityId entityId);
    EntityId GetId() const;
    ECSManager* GetManager() const;

    static Entity Create(ECSManager* manager);

    template <typename Component, typename... Args>
    inline void AddComponent(Args... args)
    {
      manager->AddComponent<Component>(id, args...);
    }

    template <typename... Components>
    void AddComponents(Components&&... components)
    {
      manager->AddComponents(id, components...);
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
