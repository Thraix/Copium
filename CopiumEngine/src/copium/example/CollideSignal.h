#pragma once

#include "copium/ecs/Signal.h"
#include "copium/ecs/Entity.h"

namespace Copium
{
  class CollideSignal : public Signal
  {
  private:
    Entity first;
    Entity second;
  public:
    CollideSignal(Entity first, Entity second)
      : first{first}, second{second}
    {}

    Entity GetFirst() const
    {
      return first;
    }

    Entity GetSecond() const
    {
      return second;
    }

    CP_SIGNAL_DECLERATION_DEFINITION();
  };
}
