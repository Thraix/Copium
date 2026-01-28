#include "copium/event/ViewportResize.h"

namespace Copium
{
  ViewportResize::ViewportResize(const BoundingBox& viewport)
    : Event(EventType::ViewportResize),
      viewport{viewport}
  {
  }

  const BoundingBox& ViewportResize::GetViewport() const
  {
    return viewport;
  }
}
