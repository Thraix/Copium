#include "copium/asset/Asset.h"

namespace Copium
{
  Asset::Asset(AssetType type)
  {
    metaData.type = type;
  }

  Asset::~Asset() = default;

  AssetHandle Asset::GetHandle() const
  {
    return metaData.handle;
  }

  AssetType Asset::GetType() const
  {
    return metaData.type;
  }

  const std::string& Asset::GetName() const
  {
    return metaData.name;
  }

  UUID Asset::GetUUID() const
  {
    return metaData.uuid;
  }

  bool Asset::isRuntime() const
  {
    return metaData.isRuntime;
  }

  Asset::operator AssetHandle() const
  {
    return metaData.handle;
  }
}