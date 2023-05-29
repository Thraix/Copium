#include "copium/asset/AssetRef.h"

#include "copium/asset/AssetManager.h"

namespace Copium
{
  AssetRef::AssetRef(AssetHandle handle)
    : handle{handle}
  {}

  AssetRef::~AssetRef()
  {
    if (refCounter.LastRef())
    {
      AssetManager::UnloadAsset(handle);
    }
  }

  AssetRef::operator AssetHandle() const
  {
    return handle;

  }
}
