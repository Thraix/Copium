#pragma once

#include "copium/asset/Asset.h"
#include "copium/asset/AssetFile.h"
#include "copium/util/Common.h"

#include <functional>
#include <map>
#include <vector>

namespace Copium
{
  class AssetManager
  {
    CP_STATIC_CLASS(AssetManager);
  private:
    using CreateAssetFunc = std::function<Asset&(const MetaFile& metaFile, const std::string& str)>;
    static std::map<std::string, CreateAssetFunc> assetTypes;
    static std::vector<std::string> assetDirs;
    static std::map<AssetId, std::unique_ptr<Asset>> assets;

    static std::map<std::string, AssetId> pathToAssetCache;
    static std::map<std::string, AssetId> nameToAssetCache;
    static AssetId assetId;
    static AssetId runtimeAssetId;

    static std::vector<AssetFile> cachedAssetFiles; // TODO: Make a set?
  public:
    static void RegisterAssetDir(std::string assetDir);
    static void UnregisterAssetDir(std::string assetDir);
    static Asset& GetAsset(AssetId id);
    static Asset& LoadAsset(const std::string& assetPath);
    static Asset& LoadAsset(const Uuid& uuid);
    static AssetId DuplicateAsset(AssetId id);
    static void UnloadAsset(AssetId id);
    static Asset& RegisterRuntimeAsset(const std::string& name, const Uuid& uuid, std::unique_ptr<Asset>&& asset);
    static const std::vector<AssetFile>& GetAssetFiles();
    static void Cleanup();

    template <typename AssetType>
    static void RegisterAssetType(const std::string& assetType)
    {
      CP_ASSERT(assetTypes.emplace(assetType, &AssetManager::CreateAsset<AssetType>).second, "Asset type already exists: %s", assetType.c_str());
      AssetFile::RegisterAssetType(assetType);
    }

    template <typename AssetT>
    static AssetT& LoadAsset(const std::string& assetPath)
    {
      AssetT* asset = dynamic_cast<AssetT*>(&LoadAsset(assetPath));
      CP_ASSERT(asset, "Invalid Asset cast");
      return *asset;
    }

    template <typename AssetT>
    static AssetT& LoadAsset(const Uuid& uuid)
    {
      AssetT* asset = dynamic_cast<AssetT*>(&LoadAsset(uuid));
      CP_ASSERT(asset, "Invalid Asset cast");
      return *asset;
    }

    template <typename AssetT>
    static AssetT& GetAsset(AssetId id) 
    {
      Asset& asset = GetAsset(id);
      AssetT* assetT = dynamic_cast<AssetT*>(&asset);
      CP_ASSERT(assetT, "Invalid Asset cast");
      return *assetT;
    }

    template <typename AssetT>
    static AssetT& RegisterRuntimeAsset(const std::string& name, const Uuid& uuid, std::unique_ptr<AssetT>&& assetT)
    {
      AssetT* ptr = assetT.release();
      Asset& asset = RegisterRuntimeAsset(name, uuid, std::unique_ptr<Asset>((Asset*)ptr));
      return *(AssetT*)&asset;
    }

    template <typename AssetT>
    static AssetT& RegisterRuntimeAsset(const std::string& name, std::unique_ptr<AssetT>&& assetT)
    {
      return RegisterRuntimeAsset(name, Uuid{}, std::move(assetT));
    }

    template <typename AssetT>
    static AssetT& RegisterRuntimeAsset(const Uuid& uuid, std::unique_ptr<AssetT>&& assetT)
    {
      return RegisterRuntimeAsset(uuid.ToString(), uuid, std::move(assetT));
    }

  private:
    static Asset& LoadAssetFromPath(const std::string& filepath);

    template <typename T>
    static Asset& CreateAsset(const MetaFile& metaFile, const std::string& metaFileClass)
    {
      AssetId id = assetId++;
      pathToAssetCache.emplace(metaFile.GetFilePath(), id);
      Asset& asset = *assets.emplace(id, std::make_unique<T>(metaFile)).first->second.get();
      asset.metaData.id = id;
      asset.metaData.name = metaFile.GetFilePath();
      asset.metaData.uuid = Uuid{metaFile.GetMetaClass(metaFileClass).GetValue("uuid")};
      asset.metaData.isRuntime = false;
      return asset;
    }
  };
}
