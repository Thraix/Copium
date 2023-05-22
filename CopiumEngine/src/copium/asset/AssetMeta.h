#pragma once

#include "copium/util/Uuid.h"

#include <stdint.h>
#include <string>

namespace Copium
{
  using AssetHandle = uint64_t;
  static constexpr int NULL_ASSET_HANDLE = 0;

  struct AssetMeta
  {
    AssetHandle handle;
    std::string name;
    Uuid uuid;
    bool isRuntime = false;
    int loadCount = 1;
  };
}