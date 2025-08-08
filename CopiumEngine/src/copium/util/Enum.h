#pragma once

#include <string>
#include <vector>

#define CP_STRINGIFY(x) #x
#define CP_TO_STRING(x) CP_STRINGIFY(x)

#define CP_ENUM_CREATOR(NameSpace, EnumName, EnumList) \
namespace NameSpace { \
  enum class EnumName \
  { \
    EnumList \
  }; \
  static const std::string& ToString(EnumName enumName) \
  { \
    static std::vector<std::string> enumNames = Copium::EnumPrinter::GetEnumNames(CP_TO_STRING(#EnumList)); \
    return enumNames[(int)enumName]; \
  } \
  \
  static std::ostream& operator<<(std::ostream& ostream, EnumName enumName) \
  { \
    return ostream << ToString(enumName); \
  } \
}



namespace Copium
{
  class EnumPrinter
  {
  public:
    static std::vector<std::string> GetEnumNames(const std::string& enumNames)
    {
      std::vector<std::string> strs;
      size_t lastPos = 0;
      size_t pos = enumNames.find(',', lastPos);
      while (pos != std::string::npos)
      {
        strs.emplace_back(enumNames.substr(lastPos, pos - lastPos));

        lastPos = pos + 1;
        while (enumNames[lastPos] == ' ') lastPos++;
        pos = enumNames.find(',', lastPos);
      }
      strs.emplace_back(enumNames.substr(lastPos));
      return strs;
    }
  };
}
