#include "copium/event/MouseMoveEvent.h"

namespace Copium
{
  MouseMoveEvent::MouseMoveEvent(glm::vec2 pos, glm::vec2 delta)
    : Event{EventType::MouseMove}, pos{pos}, delta{delta}
  {}

  glm::vec2 MouseMoveEvent::GetPos() const
  {
    return pos;
  }

  glm::vec2 MouseMoveEvent::GetDelta() const
  {
    return delta;
  }
}