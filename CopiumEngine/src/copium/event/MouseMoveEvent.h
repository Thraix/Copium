#pragma once

#include "copium/event/Event.h"

#include <glm/glm.hpp>

namespace Copium
{
  class MouseMoveEvent : public Event
  {
  private:
    glm::vec2 pos;
  public:
    MouseMoveEvent(glm::vec2 pos);

    glm::vec2 GetPos() const;
  };
}
