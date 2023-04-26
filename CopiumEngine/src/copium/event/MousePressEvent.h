#pragma once

#include "copium/event/Event.h"

#include <glm/glm.hpp>

namespace Copium
{
  class MousePressEvent : public Event
  {
  private:
    int button;
  public:
    MousePressEvent(int button);

    int GetButton() const;
  };
}