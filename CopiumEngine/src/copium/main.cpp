#include "copium/core/Application.h"
#include "copium/core/Vulkan.h"
#include "copium/util/Common.h"
#include "copium/util/Timer.h"

#include <GLFW/glfw3.h>

int main(int argc, char** argv)
{
  CP_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize the glfw context");
  for (int i = 0; i < argc; i++)
  {
    CP_INFO(argv[i]);
  }

  Copium::Vulkan::Initialize();
  {
    Copium::Application application;
    Copium::Timer timer;
    int frames = 0;
    while (application.Update())
    {
      glfwPollEvents();
      if (timer.Elapsed() >= 1.0)
      {
        CP_DEBUG("%d fps", frames);
        frames = 0;
        timer.Start();
      }
      frames++;
    }
  }
  Copium::Vulkan::Destroy();

  glfwTerminate();
  return 0;
}