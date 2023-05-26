#include "copium/event/Input.h"

#include "copium/util/Common.h"

namespace Copium
{
  bool Input::keyDownList[MAX_NUM_KEYS];
  bool Input::keyEventList[MAX_NUM_KEYS];
  bool Input::mouseDownList[MAX_NUM_MOUSE_BUTTONS];
  bool Input::mouseEventList[MAX_NUM_MOUSE_BUTTONS];
  glm::vec2 Input::mousePos{0.0f};

  bool Input::IsKeyPressed(int keyCode)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range");
    return keyEventList[keyCode] && keyDownList[keyCode];
  }

  bool Input::IsKeyReleased(int keyCode)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range");
    return keyEventList[keyCode] && !keyDownList[keyCode];
  }

  bool Input::IsKeyDown(int keyCode)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range");
    return keyDownList[keyCode];
  }

  bool Input::IsKeyUp(int keyCode)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range");
    return !keyDownList[keyCode];
  }

  bool Input::IsMousePressed(int button)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range");
    return mouseEventList[button] && mouseDownList[button];
  }

  bool Input::IsMouseReleased(int button)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range");
    return mouseEventList[button] && !mouseDownList[button];
  }

  bool Input::IsMouseDown(int button)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range");
    return mouseDownList[button];
  }

  bool Input::IsMouseUp(int button)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range");
    return !mouseDownList[button];
  }

  glm::vec2 Input::GetMousePos()
  {
    return mousePos;
  }

  void Input::OnKey(int keyCode, bool pressed)
  {
    CP_ASSERT(keyCode >= 0 && keyCode < MAX_NUM_KEYS, "KeyCode is out of range");
    keyDownList[keyCode] = pressed;
    keyEventList[keyCode] = true;
  }

  void Input::OnMouse(int button, bool pressed)
  {
    CP_ASSERT(button >= 0 && button < MAX_NUM_MOUSE_BUTTONS, "button is out of range");
    mouseDownList[button] = pressed;
    mouseEventList[button] = true;
  }

  void Input::OnMouseMove(glm::vec2 mousePos)
  {
    Input::mousePos = mousePos;
  }

  void Input::Update()
  {
    memset(keyEventList, false, sizeof(keyEventList));
  }
}
