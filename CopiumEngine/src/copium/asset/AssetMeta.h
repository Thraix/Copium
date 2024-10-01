#pragma once

#include "copium/util/Uuid.h"

#include <stdint.h>
#include <string>

namespace Copium
{
  static constexpr int NULL_ASSET_ID = 0;
  using AssetId = uint32_t;

  struct AssetMeta
  {
    AssetId id;
    std::string name;
    Uuid uuid;
    bool isRuntime = false;
    int loadCount = 1;
  };
}