#pragma once

#include "copium/util/MetaFile.h"
#include "copium/util/Uuid.h"

namespace Copium
{
  class AssetFile
  {
    friend class AssetManager;
  private:
    static std::vector<std::string> assetTypes;
    std::string path;
    Uuid uuid;
    int64_t dateModified;

  public:
    AssetFile(const std::string& path);

    bool NeedReload() const;
    void Load();

    const std::string& GetPath() const;
    Uuid GetUuid() const;
  private:
    void Load(MetaFile& metaFile, const std::string& className);
    static void RegisterAssetType(const std::string& assetType);
  };
}
