#pragma once

#include <iomanip>
#include <iostream>
#include <memory>

#include "copium/util/RuntimeException.h"
#include "copium/util/VulkanException.h"

#define CP_TERM_RED "\x1B[31m"
#define CP_TERM_GREEN "\x1B[32m"
#define CP_TERM_YELLOW "\x1B[33m"
#define CP_TERM_GRAY "\x1B[90m"
#define CP_TERM_CLEAR "\033[0m"

#define CP_DEBUG(...)                                                                                                \
  std::cout << CP_TERM_GRAY << "[DBG] " << __func__ << " : " << Copium::String::Format(__VA_ARGS__) << CP_TERM_CLEAR \
            << std::endl
#define CP_INFO(...) std::cout << "[INF] " << __func__ << " : " << Copium::String::Format(__VA_ARGS__) << std::endl
#define CP_WARN(...)                                                                                                   \
  std::cout << CP_TERM_YELLOW << "[WRN] " << __func__ << " : " << Copium::String::Format(__VA_ARGS__) << CP_TERM_CLEAR \
            << std::endl
#define CP_ERR(...)                                                                                                 \
  std::cout << CP_TERM_RED << "[ERR] " << __func__ << " : " << Copium::String::Format(__VA_ARGS__) << CP_TERM_CLEAR \
            << std::endl

// Continue traces, will not print the [XXX] tag before the log
#define CP_DEBUG_CONT(...)                                                                           \
  std::cout << CP_TERM_GRAY << "      " << std::setfill(' ') << std::setw(sizeof(__func__)) << "   " \
            << Copium::String::Format(__VA_ARGS__) << CP_TERM_CLEAR << std::endl
#define CP_INFO_CONT(...)                                                            \
  std::cout << "      " << std::setfill(' ') << std::setw(sizeof(__func__)) << "   " \
            << Copium::String::Format(__VA_ARGS__) << std::endl
#define CP_WARN_CONT(...)                                                                              \
  std::cout << CP_TERM_YELLOW << "      " << std::setfill(' ') << std::setw(sizeof(__func__)) << "   " \
            << Copium::String::Format(__VA_ARGS__) << CP_TERM_CLEAR << std::endl
#define CP_ERR_CONT(...)                                                                            \
  std::cout << CP_TERM_RED << "      " << std::setfill(' ') << std::setw(sizeof(__func__)) << "   " \
            << Copium::String::Format(__VA_ARGS__) << CP_TERM_CLEAR << std::endl

#define CP_ABORT(...)                                                    \
  do                                                                     \
  {                                                                      \
    CP_ERR("Aborted at %s:%d", __FILE__, __LINE__);                      \
    CP_ERR_CONT(__VA_ARGS__);                                            \
    throw Copium::RuntimeException(Copium::String::Format(__VA_ARGS__)); \
  } while (false)
#define CP_ASSERT(Function, ...)                                                      \
  do                                                                                  \
  {                                                                                   \
    if (!(Function))                                                                  \
    {                                                                                 \
      CP_ERR("Assertion failed at %s:%d", __FILE__, __LINE__);                        \
      CP_ERR_CONT("%s : %s", #Function, Copium::String::Format(__VA_ARGS__).c_str()); \
      throw Copium::RuntimeException(Copium::String::Format(__VA_ARGS__));            \
    }                                                                                 \
  } while (false)
#define CP_VK_ASSERT(Function, ...)                                                   \
  do                                                                                  \
  {                                                                                   \
    if (Function != VK_SUCCESS)                                                       \
    {                                                                                 \
      CP_ERR("Assertion failed at %s:%d", __FILE__, __LINE__);                        \
      CP_ERR_CONT("%s : %s", #Function, Copium::String::Format(__VA_ARGS__).c_str()); \
      throw Copium::VulkanException(Copium::String::Format(__VA_ARGS__));             \
    }                                                                                 \
  } while (false)

#define CP_UNIMPLEMENTED() CP_WARN("%s is unimplemented", __FUNCTION__)
#define CP_ABORT_UNIMPLEMENTED() CP_ABORT("%s is unimplemented", __FUNCTION__)

#define CP_STATIC_CLASS(ClassName) ClassName() = delete
#define CP_DELETE_COPY_AND_MOVE_CTOR(ClassName) \
  ClassName(ClassName&&) = delete;              \
  ClassName(const ClassName&) = delete;         \
  ClassName& operator=(ClassName&&) = delete;   \
  ClassName& operator=(const ClassName&) = delete

namespace Copium
{
  class String
  {
    CP_STATIC_CLASS(String);

  public:
    static std::string Format(const std::string& format)
    {
      return format;
    }

    template <typename... Args>
    static std::string Format(const std::string& format, Args... args)
    {
      int size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
      CP_ASSERT(size > 0, "Error during formatting");
      std::unique_ptr<char[]> buf(new char[size]);
      std::snprintf(buf.get(), size, format.c_str(), args...);
      return std::string(buf.get(), buf.get() + size - 1);
    }
  };
}
