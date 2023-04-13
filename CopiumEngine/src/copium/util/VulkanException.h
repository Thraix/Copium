#pragma once

#include "copium/util/RuntimeException.h"

#include <stdexcept>

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
