#include "copium/asset/AssetRef.h"

#include "copium/asset/AssetManager.h"

namespace Copium
{
  struct AssetHandleUnloader {
      void operator()(AssetHandle* handle) {
        AssetManager::UnloadAsset(*handle);
      }
  };

  AssetRef::AssetRef(AssetHandle handle)
    : handle{std::shared_ptr<AssetHandle>(new AssetHandle{handle}, AssetHandleUnloader{})}
  {}

  AssetRef::operator AssetHandle() const
  {
    return *handle;
  }
}
