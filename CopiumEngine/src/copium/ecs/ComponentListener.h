#pragma once

#include "copium/ecs/Config.h"

namespace Copium
{
  class ECSManager;

  template <typename Component>
  class ComponentListener
  {
  public:
    using component_type = Component;
    friend class ECSManager;

  protected:
    ECSManager* manager;

  public:
    virtual void Added(EntityId entityId, Component& component)
    {
    }

    virtual void Removed(EntityId entityId, Component& component)
    {
    }
  };
}
