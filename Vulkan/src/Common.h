#pragma once

#include "VulkanException.h"
#include <iostream>

#define TERM_RED    "\x1B[31m"
#define TERM_GREEN  "\x1B[32m"
#define TERM_YELLOW "\x1B[33m"
#define TERM_GRAY   "\x1B[90m"
#define TERM_CLEAR  "\033[0m"

#define CP_DEBUG(format, ...)      std::cout << TERM_GRAY   << "[DBG] " << StringFormat(format, __VA_ARGS__) << TERM_CLEAR << std::endl
#define CP_INFO(format, ...)       std::cout <<                "[INF] " << StringFormat(format, __VA_ARGS__) <<               std::endl
#define CP_WARN(format, ...)       std::cout << TERM_YELLOW << "[WRN] " << StringFormat(format, __VA_ARGS__) << TERM_CLEAR << std::endl
#define CP_ERR(format, ...)        std::cout << TERM_RED    << "[ERR] " << StringFormat(format, __VA_ARGS__) << TERM_CLEAR << std::endl

// Continue traces, will not print the [XXX] tag before the log
#define CP_DEBUG_CONT(format, ...) std::cout << TERM_GRAY   << "      " << StringFormat(format, __VA_ARGS__) << TERM_CLEAR << std::endl
#define CP_INFO_CONT(format, ...)  std::cout <<                "      " << StringFormat(format, __VA_ARGS__) <<               std::endl
#define CP_WARN_CONT(format, ...)  std::cout << TERM_YELLOW << "      " << StringFormat(format, __VA_ARGS__) << TERM_CLEAR << std::endl
#define CP_ERR_CONT(format, ...)   std::cout << TERM_RED    << "      " << StringFormat(format, __VA_ARGS__) << TERM_CLEAR << std::endl

#define CP_UNIMPLEMENTED() CP_WARN("%s is unimplemented", __FUNCTION__)
#define CP_ABORT(format, ...) \
  do \
  { \
    CP_ERR(format, __VA_ARGS__); \
    throw std::runtime_error(StringFormat(format, __VA_ARGS__)); \
  } while(false)
#define CP_ASSERT(Function, format, ...) \
  do \
  { \
    if(!(Function)) \
    { \
      CP_ERR(format, __VA_ARGS__); \
      throw std::runtime_error(StringFormat(format, __VA_ARGS__)); \
    } \
  } while(false)
#define CP_VK_ASSERT(Function, format, ...) \
  do \
  { \
    if(Function != VK_SUCCESS) \
    { \
      CP_ERR(format, __VA_ARGS__); \
      throw VulkanException(StringFormat(format, __VA_ARGS__)); \
    } \
  } while(false)
#define CP_DELETE_COPY_AND_MOVE_CTOR(ClassName) \
  ClassName(ClassName&&) = delete; \
  ClassName(const ClassName&) = delete; \
  ClassName& operator=(ClassName&&) = delete; \
  ClassName& operator=(const ClassName&) = delete

template<typename ... Args>
std::string StringFormat(const std::string& format, Args... args)
{
    int size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    CP_ASSERT(size > 0, "Error during formatting");
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

