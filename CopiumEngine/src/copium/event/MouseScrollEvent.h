#pragma once

#include "copium/event/Event.h"

namespace Copium
{
  class MouseScrollEvent : public Event
  {
  private:
    int scrollX;
    int scrollY;

  public:
    MouseScrollEvent(int scrollX, int scrollY);

    int GetScrollX() const;
    int GetScrollY() const;
  };
}
