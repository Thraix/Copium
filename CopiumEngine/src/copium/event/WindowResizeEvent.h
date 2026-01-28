#pragma once

#include "copium/event/Event.h"

namespace Copium
{
  class WindowResizeEvent : public Event
  {
  private:
    int width;
    int height;

  public:
    WindowResizeEvent(int width, int height);

    int GetWidth() const;
    int GetHeight() const;
  };
}
