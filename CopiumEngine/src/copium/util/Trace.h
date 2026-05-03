#pragma once

#include <iomanip>
#include <iostream>
#include <string_view>

#include "copium/util/Common.h"
#include "copium/util/RuntimeException.h"
#include "copium/util/VulkanException.h"

#define CP_DEBUG(...) Copium::Trace::Print(std::cout, Copium::TermColor::Gray, "[DBG]", __func__, __VA_ARGS__)
#define CP_INFO(...) Copium::Trace::Print(std::cout, Copium::TermColor::None, "[INF]", __func__, __VA_ARGS__)
#define CP_WARN(...) Copium::Trace::Print(std::cout, Copium::TermColor::Yellow, "[WRN]", __func__, __VA_ARGS__)
#define CP_ERR(...) Copium::Trace::Print(std::cout, Copium::TermColor::Red, "[ERR]", __func__, __VA_ARGS__)

// Continue traces, will not print the [XXX] tag before the log
#define CP_DEBUG_CONT(...) Copium::Trace::PrintCont(std::cout, Copium::TermColor::Gray, sizeof(__func__), __VA_ARGS__)
#define CP_INFO_CONT(...) Copium::Trace::PrintCont(std::cout, Copium::TermColor::None, sizeof(__func__), __VA_ARGS__)
#define CP_WARN_CONT(...) Copium::Trace::PrintCont(std::cout, Copium::TermColor::Yellow, sizeof(__func__), __VA_ARGS__)
#define CP_ERR_CONT(...) Copium::Trace::PrintCont(std::cout, Copium::TermColor::Red, sizeof(__func__), __VA_ARGS__)

#define CP_ABORT(...)                               \
  do                                                \
  {                                                 \
    CP_ERR("Aborted at {}:{}", __FILE__, __LINE__); \
    std::stringstream ss{};                         \
    Copium::Trace::Print(ss, __VA_ARGS__);          \
    CP_ERR_CONT(ss.str());                          \
    throw Copium::RuntimeException(ss.str());       \
  } while (false)

#define CP_ASSERT(Function, ...)                               \
  do                                                           \
  {                                                            \
    if (!(Function))                                           \
    {                                                          \
      CP_ERR("Assertion failed at {}:{}", __FILE__, __LINE__); \
      std::stringstream ss{};                                  \
      Copium::Trace::Print(ss, __VA_ARGS__);                   \
      CP_ERR_CONT("{} : {}", #Function, ss.str());             \
      throw Copium::RuntimeException(ss.str());                \
    }                                                          \
  } while (false)

#define CP_VK_ASSERT(Function, ...)                            \
  do                                                           \
  {                                                            \
    if (Function != VK_SUCCESS)                                \
    {                                                          \
      CP_ERR("Assertion failed at {}:{}", __FILE__, __LINE__); \
      std::stringstream ss{};                                  \
      Copium::Trace::Print(ss, __VA_ARGS__);                   \
      CP_ERR_CONT("{} : {}", #Function, ss.str());             \
      throw Copium::VulkanException(ss.str());                 \
    }                                                          \
  } while (false)

#define CP_UNIMPLEMENTED() CP_WARN("{} is unimplemented", __func__)
#define CP_ABORT_UNIMPLEMENTED() CP_ABORT("{} is unimplemented", __func__)

namespace Copium
{
  enum class TermColor
  {
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    Gray,
    Clear,
    None,
  };

  class Trace
  {
    CP_STATIC_CLASS(Trace);

  public:
    template <typename... Args>
    static std::ostream& Print(std::ostream& ostream, const std::string& format, const Args&... args)
    {
      PrintInternal(ostream, format, args...);
      return ostream;
    }

    template <typename... Args>
    static std::ostream& PrintCont(
      std::ostream& ostream, TermColor color, int padding, const std::string& format, const Args&... args)
    {
      ostream << GetColor(color) << "        " << std::setfill(' ') << std::setw(padding) << "   ";
      PrintInternal(ostream, format, args...);
      return ostream << GetColor(TermColor::Clear) << std::endl;
    }

    template <typename... Args>
    static std::ostream& Print(std::ostream& ostream,
                               TermColor color,
                               const std::string& logLevel,
                               const char* functionName,
                               const std::string& format,
                               const Args&... args)
    {
      ostream << GetColor(color) << logLevel << " " << functionName << " : ";
      PrintInternal(ostream, format, args...);
      return ostream << GetColor(TermColor::Clear) << std::endl;
    }

  private:
    static const char* GetColor(TermColor color)
    {
      switch (color)
      {
        case TermColor::Red:
          return "\x1B[31m";
        case TermColor::Green:
          return "\x1B[32m";
        case TermColor::Yellow:
          return "\x1B[33m";
        case TermColor::Blue:
          return "\x1B[34m";
        case TermColor::Magenta:
          return "\x1B[35m";
        case TermColor::Cyan:
          return "\x1B[36m";
        case TermColor::Gray:
          return "\x1B[90m";
        case TermColor::Clear:
          return "\033[0m";
        case TermColor::None:
          return "";
      }
      return "";
    }

    static size_t FindFormat(const std::string_view& format)
    {
      for (int i = 0; i < format.size() - 1; i++)
      {
        if (format[i] == '{' && format[i + 1] != '{')
        {
          i++;
        }
        if (format[i] == '{' && format[i + 1] == '}')
        {
          return i;
        }
      }
      return std::string::npos;
    }

    template <typename Arg>
    static size_t Print1(std::ostream& ostream, const std::string_view& format, const Arg& arg)
    {
      auto posFormat = format.find("{}");
      auto posOpenEscape = format.find("{{");
      auto posCloseEscape = format.find("}}");
      if (posFormat < posOpenEscape && posFormat < posCloseEscape)
      {
        ostream << format.substr(0, posFormat);
        ostream << arg;
        return posFormat + 2;
      }
      else if (posOpenEscape < posCloseEscape)
      {
        ostream << format.substr(0, posOpenEscape) << '{';
        return posOpenEscape + 2 + Print1(ostream, format.substr(posOpenEscape + 2), arg);
      }
      else if (posCloseEscape < posOpenEscape)
      {
        ostream << format.substr(0, posCloseEscape) << '}';
        return posCloseEscape + 2 + Print1(ostream, format.substr(posCloseEscape + 2), arg);
      }

      ostream << format;
      return format.size();
    }

    static size_t PrintInternal(std::ostream& ostream, const std::string& format)
    {
      auto posFormat = format.find("{}");
      auto posOpenEscape = format.find("{{");
      auto posCloseEscape = format.find("}}");
      if (posFormat < posOpenEscape && posFormat < posCloseEscape)
      {
        ostream << format.substr(0, posFormat);
        return posFormat + 2;
      }
      else if (posOpenEscape < posCloseEscape)
      {
        ostream << format.substr(0, posOpenEscape) << '{';
        return posOpenEscape + 2 + PrintInternal(ostream, format.substr(posOpenEscape + 2));
      }
      else if (posCloseEscape < posOpenEscape)
      {
        ostream << format.substr(0, posCloseEscape) << '}';
        return posCloseEscape + 2 + PrintInternal(ostream, format.substr(posCloseEscape + 2));
      }

      ostream << format;
      return format.size();
    }

    template <typename Arg1, typename... Args>
    static size_t PrintInternal(std::ostream& ostream, const std::string& format, const Arg1& arg1, const Args&... args)
    {
      size_t newPos = Print1(ostream, format, arg1);
      return PrintInternal(ostream, format.substr(newPos), args...);
    }
  };
}
