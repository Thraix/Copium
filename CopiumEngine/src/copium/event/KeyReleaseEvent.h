#pragma once

#include "copium/event/Event.h"

namespace Copium
{
  class KeyReleaseEvent : public Event
  {
  private:
    int button;

  public:
    KeyReleaseEvent(int button);

    int GetButton() const;
  };
}
