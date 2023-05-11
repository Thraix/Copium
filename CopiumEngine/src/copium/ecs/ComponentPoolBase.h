#pragma once

#include "copium/ecs/Config.h"
#include "copium/ecs/EntitySet.h"

#include <vector>

namespace Copium
{
  class ComponentPoolBase
  {
  protected:
    EntitySet entities;
  public:
    virtual ~ComponentPoolBase() = default;

    virtual size_t Size() = 0;
    virtual void Pop() = 0;
    virtual bool Erase(EntityID entity) = 0;
    std::vector<EntityID>& GetEntities();
    const std::vector<EntityID>& GetEntities() const;
  };
}
