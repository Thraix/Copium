#pragma once

#include "copium/core/Vulkan.h"
#include "copium/ecs/System.h"
#include "copium/event/Event.h"
#include "copium/event/EventSignal.h"
#include "copium/event/MouseMoveEvent.h"
#include "copium/example/Components.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Copium
{
  class MouseFollowSystem : public System<MouseFollowC, TransformC>
  {
  public:
    void RunEntity(const Signal& signal, Entity entity, MouseFollowC& mouseFollow, TransformC& transform)
    {
      if (signal.GetId() == EventSignal::GetIdStatic())
      {
        const EventSignal& eventSignal = static_cast<const EventSignal&>(signal);
        if (eventSignal.GetEvent().GetType() == EventType::MouseMove)
        {
          const MouseMoveEvent& mouseMoveEvent = static_cast<const MouseMoveEvent&>(eventSignal.GetEvent());
          float aspect = Vulkan::GetSwapChain().GetExtent().width / (float)Vulkan::GetSwapChain().GetExtent().height;
          // TODO: Get projectionViewMatrix from camera
          transform.position = glm::inverse(glm::ortho(-aspect, aspect, -1.0f, 1.0f)) * glm::vec4{mouseMoveEvent.GetPos().x, mouseMoveEvent.GetPos().y, 0.0f, 0.0f};
          transform.position -= transform.size * glm::vec2{0.5f};
        }
      }
    }
  };
}
