#include "copium/ecs/SystemPool.h"

#include <algorithm>

#include "copium/ecs/System.h"

namespace Copium
{
  SystemPool::SystemPool(ECSManager* manager)
    : manager{manager}
  {
  }

  SystemPool::~SystemPool()
  {
    for (auto& system : systemOrder)
    {
      delete system;
    }
    systemOrder.clear();
    systems.clear();
  }

  SystemOrderer& SystemPool::AddSystem(const std::type_index& systemId, System* system)
  {
    system->SetECSManager(manager);
    addQueue.emplace_back(systemId, system, SystemOrderer{systemId, this});
    queueOperationOrder.emplace_back(QueueOperation::Add);
    return std::get<2>(addQueue.back());
  }

  void SystemPool::RemoveSystem(const std::type_index& systemId)
  {
    removeQueue.emplace_back(systemId);
    queueOperationOrder.emplace_back(QueueOperation::Remove);
  }

  void SystemPool::CommitUpdates()
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
          CommitAddSystem(addQueueIndex);
          addQueueIndex++;
          break;
        }
        case QueueOperation::Remove:
        {
          CommitRemoveSystem(removeQueueIndex);
          removeQueueIndex++;
          break;
        }
      }
    }
    removeQueue.clear();
    addQueue.clear();
    queueOperationOrder.clear();
  }

  void SystemPool::Update()
  {
    for (auto& system : systemOrder)
    {
      system->Run();
    }
  }

  void SystemPool::SendSignal(Signal* signal)
  {
    signalQueue.emplace(signal);
  }

  void SystemPool::CommitSignals()
  {
    while (!signalQueue.empty())
    {
      auto& signal = signalQueue.front();
      for (auto& system : systemOrder)
      {
        if (system->IsSignalSubscribed(signal->GetUuid()))
        {
          system->HandleSignal(*signal);
        }
      }
      delete signal;
      signalQueue.pop();
    }
  }

  size_t SystemPool::Size() const
  {
    return systemOrder.size();
  }

  bool SystemPool::IsEmpty() const
  {
    return systemOrder.empty();
  }

  void SystemPool::MoveSystemAfter(const std::type_index& systemId, const std::type_index& afterSystemId)
  {
    auto it1 = systems.find(systemId);
    CP_ASSERT(it1 != systems.end(), "System with typeid=%s does not exist in SystemPool", systemId.name());
    auto it2 = systems.find(afterSystemId);
    CP_ASSERT(it2 != systems.end(), "System with typeid=%s does not exist in SystemPool", afterSystemId.name());

    auto itSystemId = std::find(systemOrder.rbegin(), systemOrder.rend(), it1->second);
    auto itAfterSystemId = std::find(systemOrder.rbegin(), systemOrder.rend(), it2->second);
    std::rotate(itSystemId, itSystemId + 1, itAfterSystemId);
  }

  void SystemPool::MoveSystemBefore(const std::type_index& systemId, const std::type_index& beforeSystemId)
  {
    auto it1 = systems.find(systemId);
    CP_ASSERT(it1 != systems.end(), "System with typeid=%s does not exist in SystemPool", systemId.name());
    auto it2 = systems.find(beforeSystemId);
    CP_ASSERT(it2 != systems.end(), "System with typeid=%s does not exist in SystemPool", beforeSystemId.name());

    auto itSystemId = std::find(systemOrder.rbegin(), systemOrder.rend(), it1->second);
    auto itBeforeSystemId = std::find(systemOrder.rbegin(), systemOrder.rend(), it2->second);

    std::rotate(itSystemId, itSystemId + 1, itBeforeSystemId + 1);
  }

  void SystemPool::CommitAddSystem(int queueIndex)
  {
    CP_ASSERT(
      queueIndex < addQueue.size(), "queueIndex=%d is greater than the addQueueSize=%d", queueIndex, addQueue.size());

    auto& [systemId, system, ordering] = addQueue[queueIndex];
    CP_ASSERT(systems.find(systemId) == systems.end(), "System with typeid=%s already exists in SystemPool");

    systems.emplace(systemId, system);
    systemOrder.emplace_back(system);
    ordering.CommitOrdering();
  }

  void SystemPool::CommitRemoveSystem(int queueIndex)
  {
    CP_ASSERT(queueIndex < removeQueue.size(),
              "queueIndex=%d is greater than the removeQueueSize=%d",
              queueIndex,
              removeQueue.size());

    const auto& systemId = removeQueue[queueIndex];
    auto it = systems.find(systemId);
    CP_ASSERT(it != systems.end(), "System with typeid=%s does not exist in SystemPool", systemId.name());

    auto itOrder = std::find(systemOrder.begin(), systemOrder.end(), it->second);
    CP_ASSERT(itOrder != systemOrder.end(), "System with typeid=%s does not exist in systemOrder", systemId.name());

    delete it->second;
    systems.erase(it);
    systemOrder.erase(itOrder);
  }
}
