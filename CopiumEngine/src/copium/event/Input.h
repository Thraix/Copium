#pragma once

#include <vector>

#include "copium/event/InputCode.h"
#include "copium/util/BoundingBox.h"

#include <glm/glm.hpp>

namespace Copium
{
  class Input
  {
  private:
    const static int MAX_NUM_KEYS = 1024;
    const static int MAX_NUM_MOUSE_BUTTONS = 64;

    static bool keyDownList[MAX_NUM_KEYS];
    static bool keyEventList[MAX_NUM_KEYS];
    static bool mouseDownList[MAX_NUM_MOUSE_BUTTONS];
    static bool mouseEventList[MAX_NUM_MOUSE_BUTTONS];
    static glm::vec2 mousePos;
    static glm::vec2 mousePosViewport;

  public:
    // Will only be true for a single frame after the KeyPressEvent/KeyReleaseEvent
    static bool IsKeyPressed(int key);
    static bool IsKeyReleased(int key);

    static bool IsKeyDown(int key);
    static bool IsKeyUp(int key);

    // Will only be true for a single frame after the MousePressEvent/MouseReleaseEvent
    static bool IsMousePressed(int button);
    static bool IsMouseReleased(int button);

    static bool IsMouseDown(int button);
    static bool IsMouseUp(int button);

    static glm::vec2 GetMouseWindowPos();
    static glm::vec2 GetMousePos();
    static glm::vec2 GetMousePosViewport();

    static void OnKey(int keyCode, bool pressed);
    static void OnMouse(int buttion, bool pressed);
    static void OnMouseMove(glm::vec2 mousePos);

    static void Update();
    static void PushViewport(const BoundingBox& viewport);
    static void PopViewport();

    static std::string KeyToString(int key);
  };
}
