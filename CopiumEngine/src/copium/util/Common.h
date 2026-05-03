#pragma once

#define CP_STATIC_CLASS(ClassName) ClassName() = delete
#define CP_DELETE_COPY_AND_MOVE_CTOR(ClassName) \
  ClassName(ClassName&&) = delete;              \
  ClassName(const ClassName&) = delete;         \
  ClassName& operator=(ClassName&&) = delete;   \
  ClassName& operator=(const ClassName&) = delete
