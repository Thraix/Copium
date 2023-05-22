#include "copium/ecs/ComponentPoolBase.h"

namespace Copium
{
  std::vector<EntityId>& ComponentPoolBase::GetEntities()
  {
    return entities.GetList();
  }

  const std::vector<EntityId>& ComponentPoolBase::GetEntities() const
  {
    return entities.GetList();
  }
}
