#include "copium/util/StringUtil.h"

namespace Copium
{
  size_t StringUtil::GetTrimStartPos(const std::string_view& str)
  {
    size_t pos = 0;
    while (pos < str.size() && (str[pos] == ' ' || str[pos] == '\t'))
      pos++;

    return pos;
  }

  size_t StringUtil::GetTrimEndPos(const std::string_view& str)
  {
    if (str.empty())
      return 0;
    size_t pos = str.size() - 1;
    while (pos > 0 && (str[pos] == ' ' || str[pos] == '\t'))
      pos--;
    return pos;
  }

  std::string_view StringUtil::Trim(const std::string_view& str)
  {
    size_t start = GetTrimStartPos(str);
    size_t end = GetTrimEndPos(str);
    if (start == str.size() || start > end)
      return "";
    return std::string_view(str.data() + start, end - start + 1);
  }

  std::string_view StringUtil::Trim(const std::string& str)
  {
    return Trim(std::string_view(str));
  }
}