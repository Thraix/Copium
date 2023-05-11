#pragma once

#include <stdint.h>
#include <numeric>

namespace Copium
{
  using EntityID = uint32_t;
  const static uint32_t MAX_NUM_ENTITIES = std::numeric_limits<uint32_t>::max();
  const static uint32_t INVALID_ENTITY = 0;
}
