#pragma once

#include "copium/event/Event.h"

#include <glm/glm.hpp>

namespace Copium
{
  class MouseReleaseEvent : public Event
  {
  private:
    int button;
  public:
    MouseReleaseEvent(int button);

    int GetButton() const;
  };
}