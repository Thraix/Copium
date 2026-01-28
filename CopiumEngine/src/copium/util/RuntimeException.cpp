#include "copium/util/RuntimeException.h"

namespace Copium
{
  RuntimeException::RuntimeException(const std::string& str)
    : errorMessage{str}
  {
  }

  const std::string& RuntimeException::GetErrorMessage() const
  {
    return errorMessage;
  }
}
