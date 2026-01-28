#include "copium/event/WindowFocusEvent.h"

namespace Copium
{
  WindowFocusEvent::WindowFocusEvent(bool focused)
    : Event{EventType::WindowFocus},
      focused{focused}
  {
  }

  bool WindowFocusEvent::IsFocused() const
  {
    return focused;
  }
}
