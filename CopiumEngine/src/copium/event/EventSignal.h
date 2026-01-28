#pragma once

#include "copium/ecs/Signal.h"
#include "copium/event/Event.h"

namespace Copium
{

  class EventSignal : public Signal
  {
  private:
    const Event& event;

  public:
    EventSignal(const Event& event);

    const Event& GetEvent() const;

    CP_SIGNAL_DECLERATION(EventSignal);
  };

}
