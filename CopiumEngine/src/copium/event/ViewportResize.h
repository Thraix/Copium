#pragma once

#include "copium/event/Event.h"
#include "copium/util/BoundingBox.h"

namespace Copium
{
  class ViewportResize : public Event
  {
    BoundingBox viewport;

  public:
    ViewportResize(const BoundingBox& viewport);

    const BoundingBox& GetViewport() const;
  };
}
