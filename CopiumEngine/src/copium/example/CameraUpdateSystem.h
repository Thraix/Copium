#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"
#include "copium/event/EventSignal.h"
#include "copium/event/WindowResizeEvent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Copium
{
  class CameraUpdateSystem : public System<CameraC, TransformC>
  {
  private:
    glm::mat4* viewMatrix;
    glm::mat4* projectionMatrix;
    glm::mat4* invPvMatrix;
  public:
    CameraUpdateSystem(glm::mat4* viewMatrix, glm::mat4* projectionMatrix, glm::mat4* invPvMatrix)
      : viewMatrix{viewMatrix}, projectionMatrix{projectionMatrix}, invPvMatrix{invPvMatrix}
    {}

    void RunEntity(Entity entity, CameraC& camera, TransformC& transform)
    {
      *projectionMatrix = glm::ortho(camera.projection.l, camera.projection.r, camera.projection.b, camera.projection.t);
      *viewMatrix = glm::translate(glm::scale(glm::mat4{1}, glm::vec3{1.0f / transform.size.x, 1.0f / transform.size.y, 1.0f}), glm::vec3{-transform.position.x, -transform.position.y, 0.0f});
      *invPvMatrix = glm::inverse((*projectionMatrix) * (*viewMatrix));
    }

    void RunEntity(const Signal& signal, Entity entity, CameraC& camera, TransformC& transform) override
    {
      if (camera.staticBoundingBox)
        return;

      if (signal.GetId() != EventSignal::GetIdStatic())
        return;

      const EventSignal& eventSignal = static_cast<const EventSignal&>(signal);
      switch (eventSignal.GetEvent().GetType())
      {
      case EventType::WindowResize:
      {
        const WindowResizeEvent& windowResizeEvent = static_cast<const WindowResizeEvent&>(eventSignal.GetEvent());
        float aspect = windowResizeEvent.GetWidth() / (float)windowResizeEvent.GetHeight();
        camera.projection.r = aspect;
        camera.projection.l = -aspect;
        break;
      }
      }
    }
  };
}
