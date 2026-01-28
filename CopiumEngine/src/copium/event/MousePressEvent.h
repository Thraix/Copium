#pragma once

#include <glm/glm.hpp>

#include "copium/event/Event.h"

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