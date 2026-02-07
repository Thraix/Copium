#pragma once

#include "copium/util/Uuid.h"

#define CP_REGISTER_SIGNAL static inline Copium::Uuid UUID = Copium::Uuid()

namespace Copium
{
  class Signal
  {
  public:
    Signal() = default;

    const Uuid& GetUuid() const
    {
      return uuid;
    }

    template <typename T>
    static void ValidateSignal()
    {
      static_assert(std::is_base_of_v<Signal, T>, "ValidateSignal : Given T is does not have Signal as base class");
      static_assert(
        has_static_uuid<T>(),
        "ValidateSignal : Given T has not defined a UUID. Did you add CP_REGISTER_SIGNAL to your Signal class?");
    }

  private:
    friend class ECSManager;
    Uuid uuid;

    template <typename, typename = void>
    struct has_static_uuid : std::false_type
    {
    };

    template <typename T>
    struct has_static_uuid<T, std::void_t<decltype(T::UUID)>> : std::true_type
    {
    };
  };
}
