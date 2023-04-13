#pragma once

#include "copium/asset/AssetMeta.h"
#include "copium/util/MetaFile.h"
#include "copium/util/UUID.h"

#include <stdint.h>

namespace Copium
{

  class Asset
  {
    friend class AssetManager;
  public:
    Asset(AssetType type);
    virtual ~Asset();

    AssetHandle GetHandle() const;
    AssetType GetType() const;
    const std::string& GetName() const;
    UUID GetUUID() const;
    bool isRuntime() const;

    operator AssetHandle() const;

  private:
    AssetMeta metaData;
  };
}
