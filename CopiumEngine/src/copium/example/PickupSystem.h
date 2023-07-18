#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"
#include "copium/example/CollideSignal.h"

namespace Copium
{
  class PickupSystem : public SystemBase
  {
    void Run() override {}

    void Run(const Signal& signal) override
    {
      if (signal.GetId() == CollideSignal::GetIdStatic())
      {
        const CollideSignal& collideSignal = static_cast<const CollideSignal&>(signal);
        if (collideSignal.GetFirst().HasComponent<PickupC>())
        {
          if (collideSignal.GetSecond().HasComponent<PlayerC>())
          {
            collideSignal.GetFirst().Destroy();
          }
        }
        else if (collideSignal.GetSecond().HasComponent<PickupC>())
        {
          if (collideSignal.GetFirst().HasComponent<PlayerC>() && collideSignal.GetFirst().HasComponent<HealthC>())
          {
            collideSignal.GetSecond().Destroy();
            collideSignal.GetFirst().GetComponent<HealthC>().current++;
            collideSignal.GetSecond().Destroy();
          }
        }
      }
    }
  };
}
