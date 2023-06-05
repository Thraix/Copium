#pragma once

#include "copium/asset/AssetMeta.h"

namespace Copium
{
  class AssetRef
  {
  private:
    std::shared_ptr<AssetHandle> handle;

  public:
    AssetRef(AssetHandle handle);

    operator AssetHandle() const;
  };

}
