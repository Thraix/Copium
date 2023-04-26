#pragma once

#include "copium/event/Event.h"

namespace Copium
{
  class WindowFocusEvent : public Event
  {
  private:
    bool focused;
  public:
    WindowFocusEvent(bool focused);

    bool IsFocused() const;
  };
}
