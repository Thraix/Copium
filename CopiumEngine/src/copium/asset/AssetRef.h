#pragma once

#include "copium/asset/AssetMeta.h"
#include "copium/util/RefCounter.h"

namespace Copium
{
  class AssetRef
  {
  private:
    AssetHandle handle;
    RefCounter refCounter;

  public:
    AssetRef(AssetHandle handle);
    ~AssetRef();

    operator AssetHandle() const;
  };

}
