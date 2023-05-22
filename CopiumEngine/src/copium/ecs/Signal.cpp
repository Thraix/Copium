#include "copium/ecs/Signal.h"

namespace Copium
{
  int Signal::GetAllocatedId()
  {
    allocatedIds++;
    return allocatedIds;
  }
}
