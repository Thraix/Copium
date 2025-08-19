#pragma once

#include "copium/asset/AssetManager.h"

namespace Copium
{
  template <typename AssetType>
  class AssetRef
  {
  public:
    AssetRef()
      : id{std::shared_ptr<AssetId>(new AssetId(NULL_ASSET_ID), AssetIdUnloader{})}
    {}

    explicit AssetRef(const std::string& assetName)
      : id{std::shared_ptr<AssetId>(new AssetId(AssetManager::LoadAsset(assetName).GetId()), AssetIdUnloader{})}
    {}

    explicit AssetRef(const Uuid& uuid)
      : id{std::shared_ptr<AssetId>(new AssetId(AssetManager::LoadAsset(uuid).GetId()), AssetIdUnloader{})}
    {}

    explicit AssetRef(AssetType& asset)
      : id{std::shared_ptr<AssetId>(new AssetId(AssetManager::DuplicateAsset(asset.GetId())), AssetIdUnloader{})}
    {}

    explicit AssetRef(std::unique_ptr<AssetType>&& runtimeAsset)
      : id{std::shared_ptr<AssetId>(new AssetId(AssetManager::RegisterRuntimeAsset(Uuid{}, std::move(runtimeAsset)).GetId()), AssetIdUnloader{})}
    {}

    AssetRef(const Uuid& uuid, std::unique_ptr<AssetType>&& runtimeAsset)
      : id{std::shared_ptr<AssetId>(new AssetId(AssetManager::RegisterRuntimeAsset(uuid, std::move(runtimeAsset)).GetId()), AssetIdUnloader{})}
    {}

    AssetRef(const std::string& name, std::unique_ptr<AssetType>&& runtimeAsset)
      : id{std::shared_ptr<AssetId>(new AssetId(AssetManager::RegisterRuntimeAsset(name, std::move(runtimeAsset)).GetId()), AssetIdUnloader{})}
    {}

    AssetRef(const std::string& name, const Uuid& uuid, std::unique_ptr<AssetType>&& runtimeAsset)
      : id{std::shared_ptr<AssetId>(new AssetId(AssetManager::RegisterRuntimeAsset(name, uuid, std::move(runtimeAsset)).GetId()), AssetIdUnloader{})}
    {}

    AssetRef(AssetId id)
      : id{std::shared_ptr<AssetId>(new AssetId(AssetManager::DuplicateAsset(id)), AssetIdUnloader{})}
    {}

    AssetId GetId() const
    {
      return *id;
    }

    AssetType& GetAsset() const
    {
      return AssetManager::GetAsset<AssetType>(*id);
    }

    bool Valid() const
    {
      return *id != NULL_ASSET_ID;
    }

  private:
    struct AssetIdUnloader {
      void operator()(AssetId* id)
      {
        if (*id != NULL_ASSET_ID)
        {
          AssetManager::UnloadAsset(*id);
        }
      }
    };
  private:
    std::shared_ptr<AssetId> id;
  };
}
