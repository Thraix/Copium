#include "copium/asset/AssetManager.h"

#include "copium/buffer/Framebuffer.h"
#include "copium/sampler/ColorAttachment.h"
#include "copium/sampler/Texture2D.h"
#include "copium/util/Common.h"
#include "copium/util/MetaFile.h"

#include <fstream>
#include <filesystem>

namespace Copium
{
  std::vector<std::string> AssetManager::assetDirs;
  std::map<std::string, AssetManager::CreateAssetFunc> AssetManager::assetTypes;
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
      std::string assetPath = assetDir + "/" + std::filesystem::absolute(it->path()).string().substr(std::filesystem::absolute(assetDirPath).string().size() + 1).c_str();
      try
      {
        CP_DEBUG("Registering Asset: %s", assetPath.c_str());
        cachedAssetFiles.emplace_back(assetPath);
      }
      catch (RuntimeException& exception)
      {
        CP_ERR("Failed to register Asset: %s", assetPath.c_str());
      }
    }
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
    CP_ASSERT(it != assets.end(), "Asset not loaded");
    return *it->second.get();
  }

  Asset& AssetManager::LoadAsset(const std::string& assetPath)
  {
    CP_DEBUG("Loading Asset: %s", assetPath.c_str());

    for (auto& dir : assetDirs)
    {
      std::string path = dir + "/" + assetPath;
      std::ifstream file{path};
      if (!file.good())
        continue;
      file.close();

      return LoadAssetFromPath(path);
    }
    CP_ABORT("Unknown Asset: %s", assetPath.c_str());
  }


  Asset& AssetManager::LoadAsset(const Uuid& uuid) 
  {
    CP_DEBUG("Loading uuid Asset: %s", uuid.ToString().c_str());
    for (auto&& assetFile : cachedAssetFiles)
    {
      if (assetFile.NeedReload())
        assetFile.Load();

      if (assetFile.GetUuid() != uuid)
        continue;

      return LoadAssetFromPath(assetFile.GetPath());
    }
    CP_ABORT("Asset not found with uuid=%s", uuid.ToString().c_str());
    // TODO: Reload the assetCache to see if a new file has appeared with that uuid
  }

  AssetHandle AssetManager::DuplicateAsset(AssetHandle handle)
  {
    auto it = assets.find(handle);
    CP_ASSERT(it != assets.end(), "Failed to find asset with handle=%d", handle);

    CP_DEBUG("Duplicating asset: %s", it->second->GetName().c_str());
    it->second->metaData.loadCount++;
    return handle;
  }

  void AssetManager::UnloadAsset(AssetHandle handle)
  {
    auto it = assets.find(handle);
    if (it == assets.end())
    {
      CP_WARN("Asset not loaded");
      return;
    }
    CP_DEBUG("Unloading Asset: %s (%d instances left)", it->second->GetName().c_str(), it->second->metaData.loadCount - 1);

    it->second->metaData.loadCount--;
    if (it->second->metaData.loadCount > 0)
      return;

    if (it->second->isRuntime())
      nameToAssetCache.erase(it->second->GetName());
    else
      pathToAssetCache.erase(it->second->GetName());
    assets.erase(it);
  }

  const std::vector<AssetFile>& AssetManager::GetAssetFiles()
  {
    return cachedAssetFiles;
  }

  void AssetManager::Cleanup() 
  {
    if (assets.empty())
      return;
    CP_WARN("Performing auto clean up of %d non unloaded assets", assets.size());
    while (!assets.empty())
    {
      UnloadAsset(assets.begin()->second->GetHandle());
    }
    CP_ASSERT(nameToAssetCache.empty(), "Name To Asset Cache not empty after full unload");
    CP_ASSERT(pathToAssetCache.empty(), "Path To Asset Cache not empty after full unload");
  }

  Asset& AssetManager::RegisterRuntimeAsset(const std::string& name, std::unique_ptr<Asset>&& asset)
  {
    CP_DEBUG("Registering Runtime Asset: %s", name.c_str());

    auto it = nameToAssetCache.find(name);
    CP_ASSERT(it == nameToAssetCache.end(), "Asset already exists: %s", name);

    AssetHandle handle = runtimeAssetHandle++;
    Asset* asset2 = assets.emplace(handle, std::move(asset)).first->second.get();
    asset2->metaData.handle = handle;
    asset2->metaData.name = name;
    asset2->metaData.uuid = Uuid();
    asset2->metaData.isRuntime = true;
    nameToAssetCache.emplace(name, handle);
    return *asset2;
  }

  Asset& AssetManager::LoadAssetFromPath(const std::string& filepath)
  {
    CP_DEBUG("Loading Asset: %s", filepath.c_str());
    auto it = pathToAssetCache.find(filepath);
    if (it != pathToAssetCache.end())
    {
      auto itAsset = assets.find(it->second);
      itAsset->second->metaData.loadCount++;
      return *itAsset->second.get();
    }

    MetaFile metaFile{filepath};
    for (auto& assetType : assetTypes)
    {
      if(metaFile.HasMetaClass(assetType.first))
        return assetType.second(metaFile, assetType.first);
    }
    CP_ABORT("Unknown Asset type: %s", filepath.c_str());
  }
}