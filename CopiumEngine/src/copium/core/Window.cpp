#include "copium/core/Window.h"

#include "copium/core/Vulkan.h"
#include "copium/event/EventDispatcher.h"
#include "copium/event/KeyPressEvent.h"
#include "copium/event/KeyReleaseEvent.h"
#include "copium/event/MouseMoveEvent.h"
#include "copium/event/MousePressEvent.h"
#include "copium/event/MouseReleaseEvent.h"
#include "copium/event/MouseScrollEvent.h"
#include "copium/event/WindowResizeEvent.h"
#include "copium/event/WindowFocusEvent.h"

namespace Copium
{
  Window::Window(const std::string& windowName, int width, int height, WindowMode mode)
    : width{width}, height{height}
  {
    InitializeWindow(windowName, width, height, mode);
    InitializeSurface();
  }

  Window::~Window()
  {
    vkDestroySurfaceKHR(Vulkan::GetInstance(), surface, nullptr);
    glfwDestroyWindow(window);
  }

  VkSurfaceKHR Window::GetSurface() const
  {
    return surface;
  }

  GLFWwindow* Window::GetWindow()
  {
    return window;
  }

  void Window::InitializeWindow(const std::string& windowName, int width, int height, WindowMode mode)
  {
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    switch (mode)
    {
    case WindowMode::Fullscreen:
    {
      GLFWmonitor* monitor = glfwGetPrimaryMonitor();
      const GLFWvidmode* mode = glfwGetVideoMode(monitor);
      window = glfwCreateWindow(mode->width, mode->height, windowName.c_str(), monitor, nullptr);
      width = mode->width;
      height = mode->height;
      break;
    }
    case WindowMode::BorderlessWindowed:
    {
      const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

      glfwWindowHint(GLFW_DECORATED, false);
      window = glfwCreateWindow(mode->width, mode->height, windowName.c_str(), nullptr, nullptr);
      width = mode->width;
      height = mode->height;
      break;
    }
    case WindowMode::Windowed:
    {
      window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
      break;
    }
    default:
      CP_ABORT("Unreachable switch case: %s", ToString(mode).c_str());
    }

    CP_ASSERT(window, "Failed to initialize glfw window");

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, MouseMoveCallback);
    glfwSetWindowFocusCallback(window, WindowFocusCallback);
    glfwSetScrollCallback(window, MouseScrollCallback);
  }

  void Window::InitializeSurface()
  {
    CP_VK_ASSERT(glfwCreateWindowSurface(Vulkan::GetInstance(), window, nullptr, &surface), "Failed to create Vulkan surface");
  }

  void Window::FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
  {
    if (width == 0 || height == 0)
      return;
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
    window->width = width;
    window->height = height;
    Vulkan::GetSwapChain().ResizeFramebuffer();
    EventDispatcher::QueueEvent(WindowResizeEvent{width, height});
  }

  void Window::KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
      EventDispatcher::QueueEvent(KeyPressEvent(key));
    else if (action == GLFW_RELEASE)
      EventDispatcher::QueueEvent(KeyReleaseEvent(key));
  }

  void Window::MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods)
  {
    if (action == GLFW_PRESS)
      EventDispatcher::QueueEvent(MousePressEvent{button});
    else if (action == GLFW_RELEASE)
      EventDispatcher::QueueEvent(MouseReleaseEvent{button});
  }

  void Window::MouseMoveCallback(GLFWwindow* glfwWindow, double xpos, double ypos)
  {
    Window* window = (Window*)glfwGetWindowUserPointer(glfwWindow);
    EventDispatcher::QueueEvent(MouseMoveEvent{glm::vec2{xpos / window->width * 2.0 - 1.0, -(ypos / window->height * 2.0 - 1.0)}});
  }

  void Window::WindowFocusCallback(GLFWwindow* glfwWindow, int focused)
  {
    EventDispatcher::QueueEvent(WindowFocusEvent{focused == GLFW_TRUE});
  }

  void Window::MouseScrollCallback(GLFWwindow* glfwWindow, double xoffset, double yoffset)
  {
    EventDispatcher::QueueEvent(MouseScrollEvent{(int)xoffset, (int)yoffset});
  }
}