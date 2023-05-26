#include "copium/core/Application.h"
#include "copium/core/Vulkan.h"
#include "copium/event/EventDispatcher.h"
#include "copium/event/Input.h"
#include "copium/util/Common.h"
#include "copium/util/Timer.h"

#include <GLFW/glfw3.h>

int main(int argc, char** argv)
{
  CP_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize the glfw context");

  Copium::Vulkan::Initialize();
  {
    Copium::Application application;
    while (application.Update())
    {
      glfwPollEvents();

      Copium::EventDispatcher::Dispatch();
      Copium::Input::Update();
    }
  }
  Copium::Vulkan::Destroy();

  glfwTerminate();
  return 0;
}