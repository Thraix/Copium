#include "copium/event/MouseMoveEvent.h"

namespace Copium
{
  MouseMoveEvent::MouseMoveEvent(glm::vec2 pos)
    : Event{EventType::MouseMove}, pos{pos}
  {}

  glm::vec2 MouseMoveEvent::GetPos() const
  {
    return pos;
  }
}