#pragma once

#include <set>

#include "copium/ecs/ECSManager.h"
#include "copium/ecs/Signal.h"

namespace Copium
{
  class System
  {
  public:
    virtual ~System() = default;
    virtual void Run() = 0;

    virtual void HandleSignal(const Signal& signal)
    {
    }

    const bool IsSignalSubscribed(const Uuid& uuid) const
    {
      return subscribedSignals.count(uuid) != 0;
    }

    template <typename S>
    void SubscribeToSignal()
    {
      Signal::ValidateSignal<S>();
      subscribedSignals.emplace(S::UUID);
    }

    template <typename S>
    void UnsubscribeToSignal()
    {
      Signal::ValidateSignal<S>();
      subscribedSignals.erase(S::UUID);
    }

    void SetECSManager(ECSManager* manager)
    {
      this->manager = manager;
    }

    template <typename S, typename... Args>
    void SendSignal(const Args&... args)
    {
      manager->SendSignal<S>(args...);
    }

  protected:
    ECSManager* manager;

  private:
    std::set<Uuid> subscribedSignals{};
  };
}
