#pragma once

#include "copium/asset/AssetMeta.h"
#include "copium/util/MetaFile.h"
#include "copium/util/UUID.h"

namespace Copium
{
  class AssetFile
  {
    friend class AssetManager;
  private:
    static std::vector<std::string> assetTypes;
    std::string path;
    UUID uuid;
    int64_t dateModified;

  public:
    AssetFile(const std::string& path);

    bool NeedReload() const;
    void Load();

    const std::string& GetPath() const;
    UUID GetUUID() const;
  private:
    void Load(const MetaFile& metaFile, const std::string& className);
    static void RegisterAssetType(const std::string& assetType);
  };
}