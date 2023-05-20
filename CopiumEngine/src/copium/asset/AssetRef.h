#pragma once

#include "copium/asset/AssetMeta.h"

namespace Copium
{
  class AssetRef
  {
  private:
    AssetHandle handle;
    int* refCounter;

  public:
    AssetRef(AssetHandle handle);
    ~AssetRef();

    AssetRef(const AssetRef& other);
    AssetRef(AssetRef&& other);

    AssetRef& operator=(const AssetRef& rhs);
    AssetRef& operator=(AssetRef&& rhs);

    operator AssetHandle() const;
  };

}
