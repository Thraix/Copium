#include "copium/event/Event.h"

namespace Copium
{

  Event::Event(EventType type)
    : type{type}
  {}

  EventType Event::GetType() const
  {
    return type;
  }
}
