#pragma once

#include "copium/util/VulkanException.h"

#include <iostream>

#define CP_TERM_RED    "\x1B[31m"
#define CP_TERM_GREEN  "\x1B[32m"
#define CP_TERM_YELLOW "\x1B[33m"
#define CP_TERM_GRAY   "\x1B[90m"
#define CP_TERM_CLEAR  "\033[0m"

#define CP_DEBUG(format, ...)      std::cout << CP_TERM_GRAY   << "[DBG] " << Copium::String::Format(format, __VA_ARGS__) << CP_TERM_CLEAR << std::endl
#define CP_INFO(format, ...)       std::cout <<                   "[INF] " << Copium::String::Format(format, __VA_ARGS__) <<               std::endl
#define CP_WARN(format, ...)       std::cout << CP_TERM_YELLOW << "[WRN] " << Copium::String::Format(format, __VA_ARGS__) << CP_TERM_CLEAR << std::endl
#define CP_ERR(format, ...)        std::cout << CP_TERM_RED    << "[ERR] " << Copium::String::Format(format, __VA_ARGS__) << CP_TERM_CLEAR << std::endl

// Continue traces, will not print the [XXX] tag before the log
#define CP_DEBUG_CONT(format, ...) std::cout << CP_TERM_GRAY   << "      " << Copium::String::Format(format, __VA_ARGS__) << CP_TERM_CLEAR << std::endl
#define CP_INFO_CONT(format, ...)  std::cout <<                   "      " << Copium::String::Format(format, __VA_ARGS__) <<               std::endl
#define CP_WARN_CONT(format, ...)  std::cout << CP_TERM_YELLOW << "      " << Copium::String::Format(format, __VA_ARGS__) << CP_TERM_CLEAR << std::endl
#define CP_ERR_CONT(format, ...)   std::cout << CP_TERM_RED    << "      " << Copium::String::Format(format, __VA_ARGS__) << CP_TERM_CLEAR << std::endl

#define CP_UNIMPLEMENTED() CP_WARN("%s is unimplemented", __FUNCTION__)
#define CP_ABORT(format, ...) \
  do \
  { \
    CP_ERR(format, __VA_ARGS__); \
    throw std::runtime_error(Copium::String::Format(format, __VA_ARGS__)); \
  } while(false)
#define CP_ASSERT(Function, format, ...) \
  do \
  { \
    if(!(Function)) \
    { \
      CP_ERR(format, __VA_ARGS__); \
      throw std::runtime_error(Copium::String::Format(format, __VA_ARGS__)); \
    } \
  } while(false)
#define CP_VK_ASSERT(Function, format, ...) \
  do \
  { \
    if(Function != VK_SUCCESS) \
    { \
      CP_ERR(format, __VA_ARGS__); \
      throw VulkanException(Copium::String::Format(format, __VA_ARGS__)); \
    } \
  } while(false)

#define CP_STATIC_CLASS(ClassName)\
  ClassName() = delete
#define CP_DELETE_COPY_AND_MOVE_CTOR(ClassName) \
  ClassName(ClassName&&) = delete; \
  ClassName(const ClassName&) = delete; \
  ClassName& operator=(ClassName&&) = delete; \
  ClassName& operator=(const ClassName&) = delete

namespace Copium
{
  class String
  {
    CP_STATIC_CLASS(String);
  public:
    template<typename ... Args>
    static std::string Format(const std::string& format, Args... args)
    {
      int size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
      CP_ASSERT(size > 0, "Format : Error during formatting");
      std::unique_ptr<char[]> buf(new char[size]);
      std::snprintf(buf.get(), size, format.c_str(), args...);
      return std::string(buf.get(), buf.get() + size - 1);
    }
  };
}
