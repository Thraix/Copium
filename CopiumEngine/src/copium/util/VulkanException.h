#pragma once

#include <string>

#include "copium/util/RuntimeException.h"

namespace Copium
{
  class VulkanException : public RuntimeException
  {
  public:
    VulkanException(const std::string& str)
      : RuntimeException{str}
    {
    }
  };
}
