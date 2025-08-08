#include "copium/event/MouseReleaseEvent.h"

namespace Copium
{
  MouseReleaseEvent::MouseReleaseEvent(int button)
    : Event{EventType::MouseRelease}, button{button}
  {}

  int MouseReleaseEvent::GetButton() const
  {
    return button;
  }
}
