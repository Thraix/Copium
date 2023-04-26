#include "copium/event/KeyPressEvent.h"

namespace Copium
{
  KeyPressEvent::KeyPressEvent(int button)
    : Event{EventType::KeyPress}, button{button}
  {}

  int KeyPressEvent::GetButton() const
  {
    return button;
  }
}