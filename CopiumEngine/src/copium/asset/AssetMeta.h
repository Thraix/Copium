#pragma once

#include "copium/util/UUID.h"

#include <stdint.h>
#include <string>

namespace Copium
{
  enum class AssetType
  {
    Pipeline,
    Texture2D,
    Sound,
  };
  using AssetHandle = uint64_t;

  struct AssetMeta
  {
    AssetHandle handle;
    AssetType type;
    std::string name;
    UUID uuid;
    bool isRuntime;
  };
}