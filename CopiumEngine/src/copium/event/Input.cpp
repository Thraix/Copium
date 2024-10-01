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

  std::string Input::KeyToString(int key)
  {
    switch (key)
    {
    case CP_KEY_SPACE:         return "Space";
    case CP_KEY_APOSTROPHE:    return "\'";
    case CP_KEY_COMMA:         return ",";
    case CP_KEY_MINUS:         return "-";
    case CP_KEY_PERIOD:        return ".";
    case CP_KEY_SLASH:         return "/";
    case CP_KEY_0:             return "0";
    case CP_KEY_1:             return "1";
    case CP_KEY_2:             return "2";
    case CP_KEY_3:             return "3";
    case CP_KEY_4:             return "4";
    case CP_KEY_5:             return "5";
    case CP_KEY_6:             return "6";
    case CP_KEY_7:             return "7";
    case CP_KEY_8:             return "8";
    case CP_KEY_9:             return "9";
    case CP_KEY_SEMICOLON:     return ";";
    case CP_KEY_EQUAL:         return "=";
    case CP_KEY_A:             return "A";
    case CP_KEY_B:             return "B";
    case CP_KEY_C:             return "C";
    case CP_KEY_D:             return "D";
    case CP_KEY_E:             return "E";
    case CP_KEY_F:             return "F";
    case CP_KEY_G:             return "G";
    case CP_KEY_H:             return "H";
    case CP_KEY_I:             return "I";
    case CP_KEY_J:             return "J";
    case CP_KEY_K:             return "K";
    case CP_KEY_L:             return "L";
    case CP_KEY_M:             return "M";
    case CP_KEY_N:             return "N";
    case CP_KEY_O:             return "O";
    case CP_KEY_P:             return "P";
    case CP_KEY_Q:             return "Q";
    case CP_KEY_R:             return "R";
    case CP_KEY_S:             return "S";
    case CP_KEY_T:             return "T";
    case CP_KEY_U:             return "U";
    case CP_KEY_V:             return "V";
    case CP_KEY_W:             return "W";
    case CP_KEY_X:             return "X";
    case CP_KEY_Y:             return "Y";
    case CP_KEY_Z:             return "Z";
    case CP_KEY_LEFT_BRACKET:  return "[";
    case CP_KEY_BACKSLASH:     return "\\";
    case CP_KEY_RIGHT_BRACKET: return "]";
    case CP_KEY_GRAVE_ACCENT:  return "Grave Accent ?";
    case CP_KEY_WORLD_1:       return "World 1 ?";
    case CP_KEY_WORLD_2:       return "World 2 ?";
    case CP_KEY_ESCAPE:        return "Escape";
    case CP_KEY_ENTER:         return "Enter";
    case CP_KEY_TAB:           return "Tab";
    case CP_KEY_BACKSPACE:     return "Return";
    case CP_KEY_INSERT:        return "Insert";
    case CP_KEY_DELETE:        return "Delete";
    case CP_KEY_RIGHT:         return "Right Arrow";
    case CP_KEY_LEFT:          return "Left Arrow";
    case CP_KEY_DOWN:          return "Down Arrow";
    case CP_KEY_UP:            return "Up Arrow";
    case CP_KEY_PAGE_UP:       return "Page Up";
    case CP_KEY_PAGE_DOWN:     return "Page Down";
    case CP_KEY_HOME:          return "Home";
    case CP_KEY_END:           return "End";
    case CP_KEY_CAPS_LOCK:     return "Caps Lock";
    case CP_KEY_SCROLL_LOCK:   return "Scroll Lock";
    case CP_KEY_NUM_LOCK:      return "Num Lock";
    case CP_KEY_PRINT_SCREEN:  return "Print Screen";
    case CP_KEY_PAUSE:         return "Pause";
    case CP_KEY_F1:            return "F1";
    case CP_KEY_F2:            return "F2";
    case CP_KEY_F3:            return "F3";
    case CP_KEY_F4:            return "F4";
    case CP_KEY_F5:            return "F5";
    case CP_KEY_F6:            return "F6";
    case CP_KEY_F7:            return "F7";
    case CP_KEY_F8:            return "F8";
    case CP_KEY_F9:            return "F9";
    case CP_KEY_F10:           return "F10";
    case CP_KEY_F11:           return "F11";
    case CP_KEY_F12:           return "F12";
    case CP_KEY_F13:           return "F13";
    case CP_KEY_F14:           return "F14";
    case CP_KEY_F15:           return "F15";
    case CP_KEY_F16:           return "F16";
    case CP_KEY_F17:           return "F17";
    case CP_KEY_F18:           return "F18";
    case CP_KEY_F19:           return "F19";
    case CP_KEY_F20:           return "F20";
    case CP_KEY_F21:           return "F21";
    case CP_KEY_F22:           return "F22";
    case CP_KEY_F23:           return "F23";
    case CP_KEY_F24:           return "F24";
    case CP_KEY_F25:           return "F25";
    case CP_KEY_KP_0:          return "Keypad 0";
    case CP_KEY_KP_1:          return "Keypad 1";
    case CP_KEY_KP_2:          return "Keypad 2";
    case CP_KEY_KP_3:          return "Keypad 3";
    case CP_KEY_KP_4:          return "Keypad 4";
    case CP_KEY_KP_5:          return "Keypad 5";
    case CP_KEY_KP_6:          return "Keypad 6";
    case CP_KEY_KP_7:          return "Keypad 7";
    case CP_KEY_KP_8:          return "Keypad 8";
    case CP_KEY_KP_9:          return "Keypad 9";
    case CP_KEY_KP_DECIMAL:    return "Keypad ,";
    case CP_KEY_KP_DIVIDE:     return "Keypad /";
    case CP_KEY_KP_MULTIPLY:   return "Keypad *";
    case CP_KEY_KP_SUBTRACT:   return "Keypad -";
    case CP_KEY_KP_ADD:        return "Keypad +";
    case CP_KEY_KP_ENTER:      return "Keypad Enter";
    case CP_KEY_KP_EQUAL:      return "Keypad =";
    case CP_KEY_LEFT_SHIFT:    return "Left Shift";
    case CP_KEY_LEFT_CONTROL:  return "Left Control";
    case CP_KEY_LEFT_ALT:      return "Left Alt";
    case CP_KEY_LEFT_SUPER:    return "Left Super";
    case CP_KEY_RIGHT_SHIFT:   return "Right Shift";
    case CP_KEY_RIGHT_CONTROL: return "Right Control";
    case CP_KEY_RIGHT_ALT:     return "Right Alt";
    case CP_KEY_RIGHT_SUPER:   return "Right Super";
    case CP_KEY_MENU:          return "Menu";
    case CP_KEY_UNBOUND:       return "Unbound";
    default:                   return "Unknown " + std::to_string(key);
    }
  }
}
