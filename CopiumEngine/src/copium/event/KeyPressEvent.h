#pragma once

#include "copium/event/Event.h"

namespace Copium
{
  class KeyPressEvent : public Event
  {
  private:
    int button;

  public:
    KeyPressEvent(int button);

    int GetButton() const;
  };
}
