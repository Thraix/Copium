#include "copium/core/Window.h"

#include "copium/core/Instance.h"
#include "copium/core/SwapChain.h"

namespace Copium
{
    Window::Window(Vulkan& vulkan, const std::string& windowName, int width, int height, Mode mode)
      : vulkan{vulkan}
    {
      InitializeWindow(windowName, width, height, mode);
      InitializeSurface();
    }

    Window::~Window()
    {
      vkDestroySurfaceKHR(vulkan.GetInstance(), surface, nullptr);
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

    void Window::InitializeWindow(const std::string& windowName, int width, int height, Mode mode)
    {
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      switch (mode)
      {
      case Mode::Fullscreen:
      {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(mode->width, mode->height, windowName.c_str(), glfwGetPrimaryMonitor(), nullptr);
        break;
      }
      case Mode::BorderlessWindowed:
      {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(mode->width, mode->height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        break;
      }
      case Mode::Windowed:
      {
        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        break;
      }
      default:
        CP_ABORT("Window : Unreachable switch case");
      }

      CP_ASSERT(window, "InitializeWindow : Failed to initialize glfw window");

      glfwSetWindowUserPointer(window, this);
      glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    }

    void Window::InitializeSurface()
    {
      CP_VK_ASSERT(glfwCreateWindowSurface(vulkan.GetInstance(), window, nullptr, &surface), "InitializeSurface : Failed to create Vulkan surface");
    }

    void Window::FramebufferResizeCallback(GLFWwindow* glfwWindow, int width, int height)
    {
      Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
      window->vulkan.GetSwapChain().ResizeFramebuffer(); // TODO: Should maybe be handled by an event system?
    }

}