#include "copium/ecs/ComponentPoolBase.h"

namespace Copium
{
  std::vector<EntityID>& ComponentPoolBase::GetEntities()
  {
    return entities.GetList();
  }

  const std::vector<EntityID>& ComponentPoolBase::GetEntities() const
  {
    return entities.GetList();
  }
}
