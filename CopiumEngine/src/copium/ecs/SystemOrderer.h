#pragma once

#include <typeindex>

namespace Copium
{
  class SystemPool;

  class SystemOrderer
  {
  private:
    std::type_index systemId;
    SystemPool* systemPool = nullptr;
  public:
    SystemOrderer(std::type_index systemId, SystemPool* systemPool);

    template <typename Other>
    void Before()
    {
      Before(typeid(Other));
    }

    template <typename Other>
    void After()
    {
      After(typeid(Other));
    }

  private:
    void Before(const std::type_index& otherSystemId);
    void After(const std::type_index& otherSystemId);
  };
}
