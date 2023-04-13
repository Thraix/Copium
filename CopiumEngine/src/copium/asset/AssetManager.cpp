#include "copium/asset/AssetManager.h"

#include "copium/sampler/Texture2D.h"
#include "copium/util/Common.h"
#include "copium/util/MetaFile.h"

#include <fstream>
#include <filesystem>

namespace Copium
{
  std::vector<std::string> AssetManager::assetDirs;
  std::map<AssetHandle, std::unique_ptr<Asset>> AssetManager::assets;
  std::map<std::string, AssetHandle> AssetManager::pathToAssetCache;
  std::map<std::string, AssetHandle> AssetManager::nameToAssetCache;
  std::vector<AssetFile> AssetManager::cachedAssetFiles;
  AssetHandle AssetManager::assetHandle = 1;
  AssetHandle AssetManager::runtimeAssetHandle = (1 << 31) + 1;

  void AssetManager::RegisterAssetDir(std::string assetDir)
  {
    if (assetDir.back() == '/')
      assetDir.pop_back();
    assetDirs.emplace_back(assetDir);
    for (std::filesystem::recursive_directory_iterator it(assetDir), end; it != end; ++it)
    {
      if (std::filesystem::is_directory(it->path()))
        continue;
      std::filesystem::path assetDirPath{assetDir};
      cachedAssetFiles.emplace_back(assetDir + "/" + std::filesystem::absolute(it->path()).string().substr(std::filesystem::absolute(assetDirPath).string().size()).c_str());
    }
    UUID uuid{};
    CP_INFO(uuid.ToString().c_str());
  }

  void AssetManager::UnregisterAssetDir(std::string assetDir)
  {
    if (assetDir.back() == '/')
      assetDir.pop_back();

    for (auto it = assetDirs.begin(); it != assetDirs.end(); ++it) 
    {
      if (*it == assetDir)
      {
        assetDirs.erase(it);
        return;
      }
    }
  }

  Asset& AssetManager::GetAsset(AssetHandle handle)
  {
    auto it = assets.find(handle);
    CP_ASSERT(it != assets.end(), "GetAsset : Asset not loaded");
    return *it->second.get();
  }

  Asset& AssetManager::LoadAsset(const std::string& assetPath)
  {
    CP_DEBUG("LoadAsset : Loading Asset: %s", assetPath.c_str());

    for (auto& dir : assetDirs)
    {
      std::string path = dir + "/" + assetPath;
      auto it = pathToAssetCache.find(path);
      if (it != pathToAssetCache.end())
        return *assets.find(it->second)->second.get();

      std::ifstream file{path};
      if (!file.good())
        continue;

      MetaFile metaFile{path};
      if (metaFile.HasMetaClass("Texture2D"))
      {
        return CreateAsset<Texture2D>(metaFile, "Texture2D");
      }
      CP_ABORT("LoadAsset : Unknown Asset type: %s/%s", dir.c_str(), assetPath.c_str());
    }
    CP_ABORT("LoadAsset : Unknown Asset: %s", assetPath.c_str());
  }


  Asset& AssetManager::LoadAsset(const UUID& uuid) 
  {
    CP_DEBUG("LoadAsset : Loading uuid Asset: %s", uuid.ToString().c_str());
    for (auto&& assetFile : cachedAssetFiles)
    {
      if (assetFile.GetUUID() != uuid)
        continue;

      if (assetFile.NeedReload())
        assetFile.Load();

      if (assetFile.GetUUID() != uuid)
        continue;

      CP_DEBUG("LoadAsset : Loading Asset: %s", assetFile.GetPath().c_str());
      auto it = pathToAssetCache.find(assetFile.GetPath());
      if (it != pathToAssetCache.end())
        return *assets.find(it->second)->second.get();

      MetaFile metaFile{assetFile.GetPath()};
      if (metaFile.HasMetaClass("Texture2D"))
      {
        return CreateAsset<Texture2D>(metaFile, "Texture2D");
      }
      CP_ABORT("LoadAsset : Unknown Asset type: %s", assetFile.GetPath().c_str());
    }
    CP_ABORT("LoadAsset : Asset not found with uuid=%s", uuid.ToString().c_str());
    // TODO: Reload the assetCache to see if a new file has appeared with that uuid
  }

  void AssetManager::UnloadAsset(AssetHandle handle)
  {
    auto it = assets.find(handle);
    CP_ASSERT(it != assets.end(), "UnloadAsset : Asset not loaded");

    if (it->second->isRuntime())
      nameToAssetCache.erase(it->second->GetName());
    else
      pathToAssetCache.erase(it->second->GetName());
    assets.erase(it);
  }

  void AssetManager::Cleanup() 
  {
    if (assets.empty())
      return;
    CP_WARN("Cleanup : Cleaning up %d loaded assets", assets.size());
    assets.clear();
    nameToAssetCache.clear();
    pathToAssetCache.clear();
  }

  Asset& AssetManager::RegisterRuntimeAsset(const std::string& name, std::unique_ptr<Asset>&& asset)
  {
    auto it = nameToAssetCache.find(name);
    CP_ASSERT(it == nameToAssetCache.end(), "RegistedRuntimeAsset : Asset already exists: %s", name);

    AssetHandle  handle = runtimeAssetHandle++;
    Asset* asset2 = assets.emplace(handle, std::move(asset)).first->second.get();
    asset2->metaData.handle = handle;
    asset2->metaData.name = name;
    asset2->metaData.uuid = UUID();
    asset2->metaData.isRuntime = true;
    nameToAssetCache.emplace(name, handle);
    return *asset2;
  }

  template <typename T>
  Asset& AssetManager::CreateAsset(const MetaFile& metaFile, const std::string& metaFileClass)
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
}