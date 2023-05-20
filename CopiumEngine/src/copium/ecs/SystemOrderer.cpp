#include "copium/ecs/SystemOrderer.h"

#include "copium/ecs/SystemPool.h"

namespace Copium
{
  SystemOrderer::SystemOrderer(std::type_index systemId, SystemPool* systemPool)
    : systemId{systemId},
      systemPool{systemPool}
  {}

  void SystemOrderer::Before(const std::type_index& otherSystemId)
  {
    systemPool->MoveSystemBefore(systemId, otherSystemId);
  }

  void SystemOrderer::After(const std::type_index& otherSystemId)
  {
    systemPool->MoveSystemAfter(systemId, otherSystemId);
  }
}