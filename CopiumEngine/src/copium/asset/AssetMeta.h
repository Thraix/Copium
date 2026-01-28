#pragma once

#include <stdint.h>

#include <string>

#include "copium/util/Uuid.h"

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