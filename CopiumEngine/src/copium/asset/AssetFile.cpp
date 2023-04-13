#include "copium/asset/AssetFile.h"

#include "copium/util/FileSystem.h"

namespace Copium
{
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
    const std::vector<std::pair<std::string, AssetType>> strToType{{"Texture2D", AssetType::Texture2D}};
    MetaFile metaFile{path};
    for (auto&& [str, type] : strToType)
    {
      if (!metaFile.HasMetaClass(str))
        continue;

      Load(metaFile, str, type);
      return;
    }
    CP_ABORT("Load : Unknown Asset type");
  }

  const std::string& AssetFile::GetPath() const
  {
    return path;
  }

  UUID AssetFile::GetUUID() const 
  {
    return uuid;
  }

  void AssetFile::Load(const MetaFile& metaFile, const std::string& className, AssetType assetType)
  {
    const MetaFileClass& metaClass = metaFile.GetMetaClass(className);
    uuid = UUID{metaClass.GetValue("uuid")};
    type = assetType;
    dateModified = FileSystem::DateModified(path);
  }
}