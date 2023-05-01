#pragma once

#include "copium/util/Common.h"
#include "copium/util/Enum.h"

#include <GLFW/glfw3.h>

#define CP_WINDOW_MODE_ENUMS Fullscreen, BorderlessWindowed, Windowed
CP_ENUM_CREATOR(Copium, WindowMode, CP_WINDOW_MODE_ENUMS);

namespace Copium
{
  class Window final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Window);
  private:
    GLFWwindow* window;
    VkSurfaceKHR surface;

  public:
    Window(const std::string& windowName, int width, int height, WindowMode mode);
    ~Window();

    VkSurfaceKHR GetSurface() const;
    GLFWwindow* GetWindow();

  private:
    void InitializeWindow(const std::string& windowName, int width, int height, WindowMode mode);
    void InitializeSurface();

    static void FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void MouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
    static void WindowFocusCallback(GLFWwindow* window, int focused);
    static void MouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
  };
}