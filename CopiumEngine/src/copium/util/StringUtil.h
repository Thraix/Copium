#pragma once

#include "copium/util/Common.h"

#include <string>
#include <string_view>

namespace Copium
{

  class StringUtil
  {
    CP_STATIC_CLASS(StringUtil);

  public:
    static std::string_view Trim(const std::string& str);
    static std::string_view Trim(const std::string_view& str);
  private:
    static size_t GetTrimStartPos(const std::string_view& str);
    static size_t GetTrimEndPos(const std::string_view& str);
  };
}
