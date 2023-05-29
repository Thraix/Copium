#pragma once

namespace Copium
{
  class RefCounter final
  {
  private:
    int* refCounter;

  public:
    RefCounter();
    ~RefCounter();

    RefCounter(RefCounter&& rhs);
    RefCounter(const RefCounter& rhs);

    RefCounter& operator=(RefCounter&& rhs);
    RefCounter& operator=(const RefCounter& rhs);

    bool Valid() const;
    bool LastRef() const;
    int Counter() const;
  };
}
