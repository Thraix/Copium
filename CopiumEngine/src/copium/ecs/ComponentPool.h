#pragma once

#include <vector>

#include "copium/ecs/ComponentListener.h"
#include "copium/ecs/ComponentPoolBase.h"
#include "copium/ecs/Config.h"
#include "copium/ecs/EntitySet.h"
#include "copium/util/Common.h"

namespace Copium
{
  template <typename Component>
  class ComponentPool : public ComponentPoolBase
  {
    using Iterator = typename std::vector<Component>::iterator;

  private:
    std::vector<Component> components;
    ComponentListener<Component>* listener = nullptr;

    enum class QueueOperation
    {
      Add,
      Remove
    };

    std::vector<QueueOperation> queueOperationOrder;
    std::vector<std::pair<EntityId, Component>> addQueue;
    std::vector<EntityId> removeQueue;

  public:
    ComponentPool()
    {
    }

    ~ComponentPool() override
    {
      if (listener)
        delete listener;
    }

    ComponentPool(EntityId entity, const Component& component)
    {
      Emplace(entity, component);
    }

    void Emplace(EntityId entity, const Component& component)
    {
      addQueue.emplace_back(entity, component);
      queueOperationOrder.emplace_back(QueueOperation::Add);
    }

    bool Erase(EntityId entity) override
    {
      if (entities.Find(entity) == entities.Size())
        return false;

      removeQueue.emplace_back(entity);
      queueOperationOrder.emplace_back(QueueOperation::Remove);

      return true;
    }

    void CommitUpdates() override
    {
      if (queueOperationOrder.empty())
        return;

      CP_ASSERT(queueOperationOrder.size() == addQueue.size() + removeQueue.size(),
                "queueOperationOrder size=%zu doesn't match the sum of the addQueue size=%zu and removeQueue size=%zu, "
                "which is %zu",
                queueOperationOrder.size(),
                addQueue.size(),
                removeQueue.size(),
                addQueue.size() + removeQueue.size());

      int addQueueIndex = 0;
      int removeQueueIndex = 0;
      for (QueueOperation queueOperation : queueOperationOrder)
      {
        switch (queueOperation)
        {
          case QueueOperation::Add:
          {
            CommitAddComponent(addQueueIndex);
            addQueueIndex++;
            break;
          }
          case QueueOperation::Remove:
          {
            CommitRemoveComponent(removeQueueIndex);
            removeQueueIndex++;
            break;
          }
        }
      }
      removeQueue.clear();
      addQueue.clear();
      queueOperationOrder.clear();
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

    void CommitAddComponent(int queueIndex)
    {
      CP_ASSERT(
        queueIndex < addQueue.size(), "queueIndex=%d is greater than the addQueueSize=%d", queueIndex, addQueue.size());

      const auto& [entity, component] = addQueue[queueIndex];
      // TODO: Debugging errors caused by this assert might be a bit difficult, since there wont be any stacktrace for
      //       where the component was added. Might want to validate this in AddComponent somehow (like looping through
      //       the queued changes and work out if the component already exists)
      CP_ASSERT(Find(entity) == Size(),
                "Component already exists in entity (entity=%u, Component=%s)",
                entity,
                typeid(Component).name());

      components.push_back(component);
      entities.Emplace(entity);
      if (listener)
        listener->Added(entity, components.back());
    }

    void CommitRemoveComponent(int queueIndex)
    {
      CP_ASSERT(queueIndex < removeQueue.size(),
                "queueIndex=%d is greater than the removeQueueSize=%d",
                queueIndex,
                removeQueue.size());

      const auto& entity = removeQueue[queueIndex];
      size_t index = entities.Find(entity);
      if (!entities.Erase(entity))
      {
        // TODO: Debugging warnings caused by this might be a bit difficult, since there wont be any stacktrace for
        //       where the component was added. Might want to validate this in AddComponent somehow (like looping
        //       through the queued changes and work out if the component already exists)
        CP_WARN("Entity did not contain component (entity=%u, Component=%s)", entity, typeid(Component).name());
        return;
      }

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
    }
  };
}
