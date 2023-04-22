#include "copium/asset/Asset.h"

namespace Copium
{
  Asset::Asset() = default;
  Asset::~Asset() = default;

  AssetHandle Asset::GetHandle() const
  {
    return metaData.handle;
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