#include "copium/util/RefCounter.h"

#include "copium/util/Common.h"

namespace Copium
{
  RefCounter::RefCounter()
    : refCounter{new int{1}}
  {}

  RefCounter::~RefCounter()
  {
    if (Valid())
    {
      (*refCounter)--;
      if (*refCounter == 0)
      {
        delete refCounter;
        refCounter = nullptr;
      }
    }
  }

  RefCounter::RefCounter(RefCounter&& rhs)
    : refCounter(rhs.refCounter)
  {
    CP_ASSERT(Valid(), "RefCounter : Moving a deleted RefCounter");
    rhs.refCounter = nullptr;
  }

  RefCounter::RefCounter(const RefCounter& rhs)
    : refCounter{rhs.refCounter}
  {
    CP_ASSERT(Valid(), "RefCounter : Copying a deleted RefCounter");
    (*refCounter)++;
  }

  RefCounter& RefCounter::operator=(RefCounter&& rhs)
  {
    CP_ASSERT(Valid(), "operator= : Moving a deleted RefCounter");

    refCounter = rhs.refCounter;
    rhs.refCounter = nullptr;
    return *this;
  }

  RefCounter& RefCounter::operator=(const RefCounter& rhs)
  {
    CP_ASSERT(Valid(), "operator= : Copying a deleted RefCounter");

    refCounter = rhs.refCounter;
    (*refCounter)++;
    return *this;
  }

  bool RefCounter::Valid() const
  {
    return refCounter != nullptr;
  }

  bool RefCounter::LastRef() const
  {
    return Valid() && *refCounter == 1;
  }

  int RefCounter::Counter() const
  {
    CP_ASSERT(Valid(), "Counter : referencing a deleted RefCounter");

    return *refCounter;
  }
}
