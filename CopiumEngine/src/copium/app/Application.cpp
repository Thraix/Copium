#include "copium/app/Application.h"

#include <GLFW/glfw3.h>

#include "copium/event/EventDispatcher.h"
#include "copium/event/Input.h"

namespace Copium
{

  void Application::Run()
  {
    while (Update())
    {
      Input::Update();
      glfwPollEvents();
      EventDispatcher::Dispatch();
    }
  }
}
