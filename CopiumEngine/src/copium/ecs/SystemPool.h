#pragma once

#include <map>
#include <queue>
#include <typeindex>
#include <vector>

#include "copium/ecs/Signal.h"
#include "copium/ecs/SystemOrderer.h"
#include "copium/util/Common.h"

namespace Copium
{
  class ECSManager;
  class System;

  class SystemPool final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(SystemPool);

  public:
    SystemPool(ECSManager* manager);
    ~SystemPool();

    SystemOrderer& AddSystem(const std::type_index& systemId, System* system);
    void RemoveSystem(const std::type_index& systemId);
    void Update();
    void SendSignal(Signal* signal);
    void CommitSignals();
    void CommitUpdates();

    size_t Size() const;
    bool IsEmpty() const;

    void MoveSystemAfter(const std::type_index& systemId, const std::type_index& afterSystemId);
    void MoveSystemBefore(const std::type_index& systemId, const std::type_index& beforeSystemId);

  private:
    ECSManager* manager;
    std::map<std::type_index, System*> systems;
    std::vector<System*> systemOrder;

    enum class QueueOperation
    {
      Add,
      Remove
    };

    std::vector<QueueOperation> queueOperationOrder;
    std::vector<std::tuple<std::type_index, System*, SystemOrderer>> addQueue;
    std::vector<std::type_index> removeQueue;
    std::queue<Signal*> signalQueue;

    void CommitAddSystem(int queueIndex);
    void CommitRemoveSystem(int queueIndex);
  };
}
