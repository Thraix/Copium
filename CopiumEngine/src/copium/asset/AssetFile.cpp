#include "copium/asset/AssetFile.h"

#include "copium/util/FileSystem.h"

namespace Copium
{
  std::vector<std::string> AssetFile::assetTypes;

  AssetFile::AssetFile(const std::string& path)
    : path{path}
  {
    Load();
  }

  bool AssetFile::NeedReload() const
  {
    return dateModified < FileSystem::DateModified(path);
  }

  void AssetFile::Load() 
  {
    MetaFile metaFile{path};
    for (auto&& assetType : assetTypes)
    {
      if (!metaFile.HasMetaClass(assetType))
        continue;

      Load(metaFile, assetType);
      return;
    }
    CP_WARN("Unknown Asset type in file: %s", metaFile.GetFilePath().c_str());
  }

  const std::string& AssetFile::GetPath() const
  {
    return path;
  }

  UUID AssetFile::GetUUID() const 
  {
    return uuid;
  }

  void AssetFile::Load(const MetaFile& metaFile, const std::string& className)
  {
    const MetaFileClass& metaClass = metaFile.GetMetaClass(className);
    uuid = UUID{metaClass.GetValue("uuid")};
    dateModified = FileSystem::DateModified(path);
  }

  void AssetFile::RegisterAssetType(const std::string& assetType)
  {
    assetTypes.emplace_back(assetType);
  }
}