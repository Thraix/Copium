#pragma once

#include "copium/core/Vulkan.h"
#include "copium/util/Common.h"

#include <GLFW/glfw3.h>

namespace Copium
{
  class Window final
  {
    CP_DELETE_COPY_AND_MOVE_CTOR(Window);
  public:
    enum class Mode
    {
      Fullscreen, BorderlessWindowed, Windowed
    };
  private:
    Vulkan& vulkan;

    GLFWwindow* window;
    VkSurfaceKHR surface;

  public:
    Window(Vulkan& vulkan, const std::string& windowName, int width, int height, Mode mode);
    ~Window();

    VkSurfaceKHR GetSurface() const;
    GLFWwindow* GetWindow();

  private:
    void InitializeWindow(const std::string& windowName, int width, int height, Mode mode);
    void InitializeSurface();
    static void FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height);

  };
}