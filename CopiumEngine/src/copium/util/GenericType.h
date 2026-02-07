#pragma once

#include <functional>

class GenericType
{
public:
  template <typename T, typename... Args>
  static GenericType Create(const Args&... args)
  {
    GenericType type;
    type.data = new T{args...};
    type.deleter = [](void* data) { delete (T*)data; };

    return type;
  }

  GenericType(GenericType&& type)
    : data{type.data},
      deleter{type.deleter}
  {
    type.data = nullptr;
  }

  GenericType& operator=(GenericType&& type)
  {
    this->data = type.data;
    this->deleter = type.deleter;
    type.data = nullptr;
    return *this;
  }

  ~GenericType()
  {
    if (data)
    {
      deleter(data);
    }
    data = nullptr;
  }

  template <typename T>
  T& Get()
  {
    return *(T*)data;
  }

private:
  GenericType() = default;

  GenericType(const GenericType& type) = delete;
  GenericType& operator=(const GenericType& type) = delete;

  void* data;
  std::function<void(void*)> deleter;
};
