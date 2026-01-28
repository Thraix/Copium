#include "copium/event/WindowResizeEvent.h"

namespace Copium
{
  WindowResizeEvent::WindowResizeEvent(int width, int height)
    : Event{EventType::WindowResize},
      width{width},
      height{height}
  {
  }

  int WindowResizeEvent::GetWidth() const
  {
    return width;
  }

  int WindowResizeEvent::GetHeight() const
  {
    return height;
  }
}
