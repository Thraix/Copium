#pragma once

#include "copium/asset/AssetMeta.h"
#include "copium/util/MetaFile.h"
#include "copium/util/Uuid.h"

#include <stdint.h>

namespace Copium
{

  class Asset
  {
    friend class AssetManager;
  public:
    Asset();
    virtual ~Asset();

    AssetHandle GetHandle() const;
    const std::string& GetName() const;
    Uuid GetUuid() const;
    bool isRuntime() const;

    operator AssetHandle() const;

  private:
    AssetMeta metaData;
  };
}
