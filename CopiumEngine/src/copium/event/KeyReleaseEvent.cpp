#include "copium/event/KeyReleaseEvent.h"

namespace Copium
{
  KeyReleaseEvent::KeyReleaseEvent(int button)
    : Event{EventType::KeyRelease}, button{button}
  {}

  int KeyReleaseEvent::GetButton() const
  {
    return button;
  }
}