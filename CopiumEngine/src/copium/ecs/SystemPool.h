#pragma once

#include <map>
#include <typeindex>
#include <vector>

#include "copium/ecs/Signal.h"
#include "copium/ecs/SystemBase.h"
#include "copium/ecs/SystemOrderer.h"
#include "copium/util/Common.h"

namespace Copium
{
  class ECSManager;

  class SystemPool final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(SystemPool);

  private:
    ECSManager* manager;
    std::map<std::type_index, SystemBase*> systems;
    std::vector<SystemBase*> systemOrder;

  public:
    SystemPool(ECSManager* manager);
    ~SystemPool();

    SystemOrderer AddSystem(const std::type_index& systemId, SystemBase* system);
    void RemoveSystem(const std::type_index& systemId);
    void Update();
    void Update(const Signal& signal);

    void MoveSystemAfter(const std::type_index& systemId, const std::type_index& afterSystemId);
    void MoveSystemBefore(const std::type_index& systemId, const std::type_index& beforeSystemId);
  };
}
