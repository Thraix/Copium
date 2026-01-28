#pragma once

#define CP_SIGNAL_DECLERATION_DEFINITION() \
  static int GetIdStatic()                 \
  {                                        \
    static int id = GetAllocatedId();      \
    return id;                             \
  }                                        \
                                           \
  int GetId() const                        \
  {                                        \
    return GetIdStatic();                  \
  }

#define CP_SIGNAL_DECLERATION(SignalClass) \
  static int GetIdStatic();                \
  int GetId() const override

#define CP_SIGNAL_DEFINITION(SignalClass) \
  int SignalClass::GetIdStatic()          \
  {                                       \
    static int id = GetAllocatedId();     \
    return id;                            \
  }                                       \
                                          \
  int SignalClass::GetId() const          \
  {                                       \
    return GetIdStatic();                 \
  }

namespace Copium
{
  class Signal
  {
  private:
    static inline int allocatedIds = 0;

  public:
    Signal() = default;
    virtual ~Signal() = default;

    virtual int GetId() const = 0;

  protected:
    static int GetAllocatedId();
  };

}
