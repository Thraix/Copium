#pragma once

#include "copium/asset/AssetManager.h"
#include "copium/asset/AssetMeta.h"

namespace Copium
{
  template <typename AssetType>
  class AssetHandle
  {
  public:
    AssetHandle()
      : id{NULL_ASSET_ID}
    {
    }

    AssetHandle(const std::string& assetName)
      : id{AssetManager::LoadAsset(assetName).GetId()}
    {
    }

    AssetHandle(const Uuid& uuid)
      : id{AssetManager::LoadAsset(uuid).GetId()}
    {
    }

    AssetHandle(AssetType& asset)
      : id{AssetManager::DuplicateAsset(asset.GetId())}
    {
    }

    AssetHandle(const std::string& name, std::unique_ptr<AssetType>&& runtimeAsset)
      : id{AssetManager::RegisterRuntimeAsset(name, std::move(runtimeAsset)).GetId()}
    {
    }

    AssetHandle(AssetId id)
      : id{AssetManager::DuplicateAsset(id)}
    {
    }

    AssetId GetId() const
    {
      return id;
    }

    AssetType& GetAsset() const
    {
      return AssetManager::GetAsset<AssetType>(id);
    }

    void UnloadAsset()
    {
      AssetManager::UnloadAsset(id);
      id = NULL_ASSET_ID;
    }

    bool Valid() const
    {
      return id != NULL_ASSET_ID;
    }

  private:
    AssetId id;
  };
}
