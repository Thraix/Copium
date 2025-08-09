#pragma once

#include "copium/util/RuntimeException.h"

#include <string>

namespace Copium
{
  class VulkanException : public RuntimeException
  {
  public:
    VulkanException(const std::string& str)
      : RuntimeException{str}
    {}
  };
}
