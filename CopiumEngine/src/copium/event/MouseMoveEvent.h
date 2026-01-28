#pragma once

#include <glm/glm.hpp>

#include "copium/event/Event.h"

namespace Copium
{
  class MouseMoveEvent : public Event
  {
  private:
    glm::vec2 pos;
    glm::vec2 delta;

  public:
    MouseMoveEvent(glm::vec2 pos, glm::vec2 delta);

    glm::vec2 GetPos() const;
    glm::vec2 GetDelta() const;
  };
}
