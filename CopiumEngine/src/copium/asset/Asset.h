#pragma once

#include "copium/asset/AssetMeta.h"
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

    const std::string& GetName() const;
    Uuid GetUuid() const;
    bool IsRuntime() const;
    AssetId GetId() const;

  private:
    AssetMeta metaData;
  };
}
