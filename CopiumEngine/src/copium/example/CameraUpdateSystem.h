#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"
#include "copium/event/EventSignal.h"
#include "copium/event/ViewportResize.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Copium
{
  class CameraUpdateSystem : public System<CameraC, TransformC>
  {
  private:
    glm::mat4* viewMatrix;
    glm::mat4* projectionMatrix;
    glm::mat4* invPvMatrix;
    glm::mat4* uiProjectionMatrix;
  public:
    CameraUpdateSystem(glm::mat4* viewMatrix, glm::mat4* projectionMatrix, glm::mat4* invPvMatrix, glm::mat4* uiProjectionMatrix)
      : viewMatrix{viewMatrix}, projectionMatrix{projectionMatrix}, invPvMatrix{invPvMatrix}, uiProjectionMatrix{uiProjectionMatrix}
    {}

    void RunEntity(Entity entity, CameraC& camera, TransformC& transform)
    {
      if (camera.uiCamera)
      {
        *uiProjectionMatrix = glm::ortho(camera.projection.l, camera.projection.r, camera.projection.b, camera.projection.t);
      }
      else
      {
        *projectionMatrix = glm::ortho(camera.projection.l, camera.projection.r, camera.projection.b, camera.projection.t);
        *viewMatrix = glm::translate(glm::scale(glm::mat4{1}, glm::vec3{1.0f / transform.size.x, 1.0f / transform.size.y, 1.0f}), glm::vec3{-transform.position.x, -transform.position.y, 0.0f});
        *invPvMatrix = glm::inverse((*projectionMatrix) * (*viewMatrix));
      }
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
      case EventType::ViewportResize:
      {
        const ViewportResize& viewportResize = static_cast<const ViewportResize&>(eventSignal.GetEvent());
        if (camera.uiCamera)
        {
          camera.projection.r = viewportResize.GetViewport().GetSize().x;
          camera.projection.t = viewportResize.GetViewport().GetSize().y;
          camera.projection.l = 0.0f;
          camera.projection.b = 0.0f;
        }
        else
        {
          float aspect = viewportResize.GetViewport().GetSize().x / viewportResize.GetViewport().GetSize().y;
          camera.projection.r = aspect;
          camera.projection.l = -aspect;
        }
        break;
      }
      }
    }
  };
}
