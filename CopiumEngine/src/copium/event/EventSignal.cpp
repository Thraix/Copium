#include "copium/event/EventSignal.h"

namespace Copium
{
  EventSignal::EventSignal(const Event& event)
    : event{event}
  {
  }

  const Event& EventSignal::GetEvent() const
  {
    return event;
  }

  CP_SIGNAL_DEFINITION(EventSignal);
}
