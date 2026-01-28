#pragma once

#include "copium/ecs/Signal.h"

namespace Copium
{
  class ECSManager;

  class SystemBase
  {
    friend class SystemPool;

  protected:
    ECSManager* manager;

  public:
    virtual ~SystemBase() = default;

    virtual void Run() = 0;
    virtual void Run(const Signal& signal) = 0;
  };
}
