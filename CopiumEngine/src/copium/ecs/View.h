#pragma once

#include "copium/ecs/ECSManager.h"
#include "copium/ecs/Entity.h"

namespace Copium
{
  template <typename Component, typename... Components>
  struct View
  {
    class Iterator
    {
    public:
      std::tuple<Entity, Component&, Components&...> operator*()
      {
        EntityId entityId{entities[index]};
        CP_ASSERT((manager->HasComponents<Component, Components...>(entityId)),
                  "entity doesn't contains all components");
        return std::forward_as_tuple<Entity, Component&, Components&...>(
          Entity{manager, entityId}, firstPool->At(index), manager->GetComponent<Components>(entityId)...);
      }

      Iterator& operator++()
      {
        ++index;
        FindNextEntity();
        return *this;
      }

      Iterator operator++(int)
      {
        Iterator it = *this;
        ++*this;
        return it;
      }

      Iterator operator+(size_t advance)
      {
        Iterator it = *this;
        for (int i = 0; i < advance; i++)
          ++it;
        return it;
      }

      bool operator==(const Iterator& other) const
      {
        return index == other.index;
      }

      bool operator!=(const Iterator& other) const
      {
        return !(*this == other);
      }

    private:
      friend class View;

      ECSManager* manager;
      ComponentPool<std::remove_const_t<Component>>* firstPool;
      const std::vector<EntityId>& entities;
      int index;

      Iterator(ECSManager* manager, ComponentPool<std::remove_const_t<Component>>* firstPool)
        : manager{manager},
          firstPool{firstPool},
          entities{firstPool ? firstPool->GetEntities() : ECSManager::emptyEntities},
          index{0}
      {
      }

      static Iterator Begin(ECSManager* manager)
      {
        Iterator iterator{manager, manager->GetComponentPool<Component>()};
        iterator.FindNextEntity();
        return iterator;
      }

      static Iterator End(ECSManager* manager)
      {
        Iterator iterator{manager, manager->GetComponentPool<Component>()};
        if (iterator.firstPool)
          iterator.index = iterator.firstPool->GetEntities().size();
        return iterator;
      }

      void FindNextEntity()
      {
        while (index < entities.size() && !manager->HasComponents<Components...>(entities[index]))
        {
          ++index;
        }
      }
    };

    View(ECSManager* manager)
      : manager{manager}
    {
    }

    Iterator begin()
    {
      return Iterator::Begin(manager);
    }

    Iterator end()
    {
      return Iterator::End(manager);
    }

  private:
    ECSManager* manager;
  };
}
