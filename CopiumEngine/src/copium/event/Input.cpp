#include "copium/event/Input.h"

#include "copium/util/Common.h"
#include "copium/core/Vulkan.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Copium
{
  bool Input::keyDownList[MAX_NUM_KEYS];
  bool Input::keyEventList[MAX_NUM_KEYS];
  bool Input::mouseDownList[MAX_NUM_MOUSE_BUTTONS];
  bool Input::mouseEventList[MAX_NUM_MOUSE_BUTTONS];
  glm::vec2 Input::mousePos{0.0f};
  glm::vec2 Input::mousePosViewport{0.0f};

  bool Input::IsKeyPressed(int keyCode)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range %d", keyCode);
    return keyEventList[keyCode] && keyDownList[keyCode];
  }

  bool Input::IsKeyReleased(int keyCode)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range %d", keyCode);
    return keyEventList[keyCode] && !keyDownList[keyCode];
  }

  bool Input::IsKeyDown(int keyCode)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range %d", keyCode);
    return keyDownList[keyCode];
  }

  bool Input::IsKeyUp(int keyCode)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range %d", keyCode);
    return !keyDownList[keyCode];
  }

  bool Input::IsMousePressed(int button)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range %d", button);
    return mouseEventList[button] && mouseDownList[button];
  }

  bool Input::IsMouseReleased(int button)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range %d", button);
    return mouseEventList[button] && !mouseDownList[button];
  }

  bool Input::IsMouseDown(int button)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range %d", button);
    return mouseDownList[button];
  }

  bool Input::IsMouseUp(int button)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range %d", button);
    return !mouseDownList[button];
  }

  glm::vec2 Input::GetMousePos()
  {
    return mousePos;
  }

  glm::vec2 Input::GetMousePosViewport()
  {
    return mousePosViewport;
  }

  glm::vec2 Input::GetMouseWindowPos()
  {
    return glm::vec2{(mousePos.x + 1.0f) * 0.5f * Vulkan::GetWindow().GetWidth(), (1.0f - mousePos.y) * 0.5f * Vulkan::GetWindow().GetHeight()};
  }

  void Input::OnKey(int keyCode, bool pressed)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range %d", keyCode);
    keyDownList[keyCode] = pressed;
    keyEventList[keyCode] = true;
  }

  void Input::OnMouse(int button, bool pressed)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range %d", button);
    mouseDownList[button] = pressed;
    mouseEventList[button] = true;
  }

  void Input::OnMouseMove(glm::vec2 mousePos)
  {
    Input::mousePos = mousePos;
    Input::mousePosViewport = mousePos;
  }

  void Input::Update()
  {
    memset(keyEventList, false, sizeof(keyEventList));
    memset(mouseEventList, false, sizeof(mouseEventList));
  }

  void Input::PushViewport(const BoundingBox& viewport)
  {
    mousePosViewport = GetMouseWindowPos();
    mousePosViewport.x = (mousePosViewport.x - viewport.l) / viewport.GetSize().x * 2 - 1;
    mousePosViewport.y = 1 - (mousePosViewport.y - viewport.t) / viewport.GetSize().y * 2;
  }

  void Input::PopViewport()
  {
    mousePosViewport = mousePos;
  }
}
