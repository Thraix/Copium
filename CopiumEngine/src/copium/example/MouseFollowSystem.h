#pragma once

#include "copium/core/Vulkan.h"
#include "copium/ecs/System.h"
#include "copium/event/Event.h"
#include "copium/event/EventSignal.h"
#include "copium/event/MouseMoveEvent.h"
#include "copium/example/Components.h"

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
          transform.position = {(mouseMoveEvent.GetPos().x / Vulkan::GetSwapChain().GetExtent().width - 0.5) * 2.0 * aspect,
                                -(mouseMoveEvent.GetPos().y / Vulkan::GetSwapChain().GetExtent().height - 0.5) * 2.0};
          transform.position -= transform.size * glm::vec2{0.5f};
        }
      }
    }
  };
}
