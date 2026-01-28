#include "copium/event/MousePressEvent.h"

namespace Copium
{
  MousePressEvent::MousePressEvent(int button)
    : Event{EventType::MousePress},
      button{button}
  {
  }

  int MousePressEvent::GetButton() const
  {
    return button;
  }
}
