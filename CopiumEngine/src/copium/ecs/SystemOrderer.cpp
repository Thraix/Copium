#include "copium/ecs/SystemOrderer.h"

#include "copium/ecs/SystemPool.h"

namespace Copium
{
  SystemOrderer::SystemOrderer(std::type_index systemId, SystemPool* systemPool)
    : systemId{systemId},
      systemPool{systemPool}
  {
  }

  void SystemOrderer::CommitOrdering()
  {
    for (const auto& [orderOperation, systemId] : orderQueue)
    {
      switch (orderOperation)
      {
        case OrderOperation::Before:
        {
          CommitBefore(systemId);
          break;
        }
        case OrderOperation::After:
        {
          CommitAfter(systemId);
          break;
        }
      }
    }
    orderQueue.clear();
  }

  void SystemOrderer::CommitBefore(const std::type_index& otherSystemId)
  {
    systemPool->MoveSystemBefore(systemId, otherSystemId);
  }

  void SystemOrderer::CommitAfter(const std::type_index& otherSystemId)
  {
    systemPool->MoveSystemAfter(systemId, otherSystemId);
  }
}
