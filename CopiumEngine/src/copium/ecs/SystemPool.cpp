#include "copium/ecs/SystemPool.h"

namespace Copium
{
  SystemPool::SystemPool(ECSManager* manager)
    : manager{manager}
  {}

  SystemPool::~SystemPool()
  {
    for (auto& system : systemOrder)
    {
      delete system;
    }
    systemOrder.clear();
    systems.clear();
  }

  SystemOrderer SystemPool::AddSystem(const std::type_index& systemId, SystemBase* system)
  {
    system->manager = manager;
    systems.emplace(systemId, system);
    systemOrder.emplace_back(system);
    return SystemOrderer{systemId, this};
  }

  void SystemPool::Update()
  {
    for (auto& system : systemOrder)
    {
      system->Run();
    }
  }

  void SystemPool::Update(const Signal& signal)
  {
    for (auto& system : systemOrder)
    {
      system->Run(signal);
    }
  }

  void SystemPool::MoveSystemAfter(const std::type_index& systemId, const std::type_index& afterSystemId)
  {
    auto it1 = systems.find(systemId);
    CP_ASSERT(it1 != systems.end(), "System does not exist in SystemPool");
    auto it2 = systems.find(afterSystemId);
    CP_ASSERT(it2 != systems.end(), "System does not exist in SystemPool");

    auto itSystemId = std::find(systemOrder.rbegin(), systemOrder.rend(), it1->second);
    auto itAfterSystemId = std::find(systemOrder.rbegin(), systemOrder.rend(), it2->second);
    std::rotate(itSystemId, itSystemId + 1, itAfterSystemId);
  }

  void SystemPool::MoveSystemBefore(const std::type_index& systemId, const std::type_index& beforeSystemId)
  {
    auto it1 = systems.find(systemId);
    CP_ASSERT(it1 != systems.end(), "System does not exist in SystemPool");
    auto it2 = systems.find(beforeSystemId);
    CP_ASSERT(it2 != systems.end(), "System does not exist in SystemPool");

    auto itSystemId = std::find(systemOrder.rbegin(), systemOrder.rend(), it1->second);
    auto itBeforeSystemId = std::find(systemOrder.rbegin(), systemOrder.rend(), it2->second);

    std::rotate(itSystemId, itSystemId + 1, itBeforeSystemId + 1);
  }
}
