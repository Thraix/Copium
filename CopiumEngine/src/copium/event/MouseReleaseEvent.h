#pragma once

#include <glm/glm.hpp>

#include "copium/event/Event.h"

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