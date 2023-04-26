#pragma once

#include "copium/event/EventType.h"

namespace Copium
{
  class Event
  {
  private:
    EventType type;
  public:
    Event(EventType type);

    EventType GetType() const;
  };
}
