#include "copium/event/MouseScrollEvent.h"

namespace Copium
{
  MouseScrollEvent::MouseScrollEvent(int scrollX, int scrollY)
    : Event{EventType::MouseScroll}, scrollX{scrollX}, scrollY{scrollY}
  {}

  int MouseScrollEvent::GetScrollX() const
  {
    return scrollX;
  }

  int MouseScrollEvent::GetScrollY() const
  {
    return scrollY;
  }
}
