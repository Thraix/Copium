#include "copium/asset/AssetFile.h"

#include "copium/util/FileSystem.h"

#include <fstream>

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

  Uuid AssetFile::GetUuid() const
  {
    return uuid;
  }

  void AssetFile::Load(MetaFile& metaFile, const std::string& className)
  {
    MetaFileClass& metaClass = metaFile.GetMetaClass(className);
    if (!metaClass.HasValue("uuid"))
    {
      CP_WARN("Asset (%s) has no UUID assigned, generating new one", path.c_str());
      metaClass.AddValue("uuid", Uuid{}.ToString());
      std::fstream file{path};
      file << metaFile;
    }
    uuid = Uuid{metaClass.GetValue("uuid")};
    dateModified = FileSystem::DateModified(path);
  }

  void AssetFile::RegisterAssetType(const std::string& assetType)
  {
    assetTypes.emplace_back(assetType);
  }
}
