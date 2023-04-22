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
    static std::map<AssetHandle, std::unique_ptr<Asset>> assets;

    static std::map<std::string, AssetHandle> pathToAssetCache;
    static std::map<std::string, AssetHandle> nameToAssetCache;
    static AssetHandle assetHandle;
    static AssetHandle runtimeAssetHandle;

    static std::vector<AssetFile> cachedAssetFiles; // TODO: Make a set?
  public:
    static void RegisterAssetDir(std::string assetDir);
    static void UnregisterAssetDir(std::string assetDir);
    static Asset& GetAsset(AssetHandle handle);
    static Asset& LoadAsset(const std::string& assetPath);
    static Asset& LoadAsset(const UUID& uuid);
    static void UnloadAsset(AssetHandle handle);
    static Asset& RegisterRuntimeAsset(const std::string& name, std::unique_ptr<Asset>&& asset);
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
    static AssetT& LoadAsset(const UUID& uuid)
    {
      AssetT* asset = dynamic_cast<AssetT*>(&LoadAsset(uuid));
      CP_ASSERT(asset, "Invalid Asset cast");
      return *asset;
    }

    template <typename AssetT>
    static AssetT& GetAsset(AssetHandle handle) 
    {
      Asset& asset = GetAsset(handle);
      AssetT* assetT = dynamic_cast<AssetT*>(&asset);
      CP_ASSERT(assetT, "Invalid Asset cast");
      return *assetT;
    }

    template <typename AssetT>
    static AssetT& RegisterRuntimeAsset(const std::string& name, std::unique_ptr<AssetT>&& assetT) 
    {
      AssetT* ptr = assetT.release();
      Asset& asset = RegisterRuntimeAsset(name, std::unique_ptr<Asset>((Asset*)ptr));
      return *(AssetT*)&asset;
    }

  private:
    static Asset& LoadAssetFromPath(const std::string& filepath);

    template <typename T>
    static Asset& CreateAsset(const MetaFile& metaFile, const std::string& metaFileClass)
    {
      AssetHandle handle = assetHandle++;
      pathToAssetCache.emplace(metaFile.GetFilePath(), handle);
      Asset& asset = *assets.emplace(handle, std::make_unique<T>(metaFile)).first->second.get();
      asset.metaData.handle = handle;
      asset.metaData.name = metaFile.GetFilePath();
      asset.metaData.uuid = UUID{metaFile.GetMetaClass(metaFileClass).GetValue("uuid")};
      asset.metaData.isRuntime = false;
      return asset;
    }
  };
}
