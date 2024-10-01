#include "copium/asset/Asset.h"

namespace Copium
{
  Asset::Asset() = default;
  Asset::~Asset() = default;

  const std::string& Asset::GetName() const
  {
    return metaData.name;
  }

  Uuid Asset::GetUuid() const
  {
    return metaData.uuid;
  }

  bool Asset::IsRuntime() const
  {
    return metaData.isRuntime;
  }

  AssetId Asset::GetId() const
  {
    return metaData.id;
  }
}