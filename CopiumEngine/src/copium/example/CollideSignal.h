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
    bool resolved;
    int xDir;
    int yDir;
  public:
    CollideSignal(Entity first, Entity second, bool resolved, int xDir, int yDir)
      : first{first}, second{second}, resolved{resolved}, xDir{xDir}, yDir{yDir}
    {}

    Entity GetFirst() const
    {
      return first;
    }

    Entity GetSecond() const
    {
      return second;
    }

    bool WasResolved() const
    {
      return resolved;
    }

    int GetXDir() const
    {
      return xDir;
    }

    int GetYDir() const
    {
      return yDir;
    }

    CP_SIGNAL_DECLERATION_DEFINITION();
  };
}
