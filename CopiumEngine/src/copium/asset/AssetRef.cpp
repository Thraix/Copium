#include "copium/asset/AssetRef.h"

#include "copium/asset/AssetManager.h"

namespace Copium
{
  AssetRef::AssetRef(AssetHandle handle)
    : handle{handle}, refCounter{new int{1}}
  {}

  AssetRef::~AssetRef()
  {
    if (refCounter == nullptr)
      return;

    (*refCounter)--;
    if (*refCounter == 0) 
    {
      AssetManager::UnloadAsset(handle);
      delete refCounter;
    }
  }

  AssetRef::AssetRef(const AssetRef& other)
    : handle{other.handle}, refCounter{other.refCounter}
  {
    (*refCounter)++;
  }

  AssetRef::AssetRef(AssetRef&& other)
    : handle{other.handle}, refCounter{other.refCounter}
  {
    other.refCounter = nullptr;
  }

  AssetRef& AssetRef::operator=(const AssetRef& rhs)
  {
    handle = rhs.handle;
    refCounter = rhs.refCounter;

    (*refCounter)++;
    return *this;
  }

  AssetRef& AssetRef::operator=(AssetRef&& rhs)
  {
    handle = rhs.handle;
    refCounter = rhs.refCounter;
    rhs.refCounter = nullptr;
    return *this;
  }

  AssetRef::operator AssetHandle() const
  {
    return handle;

  }
}
