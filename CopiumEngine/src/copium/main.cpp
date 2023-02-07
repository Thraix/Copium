#include "copium/core/Application.h"
#include "copium/util/Common.h"
#include "copium/util/Timer.h"

#include <GLFW/glfw3.h>

int main()
{
  CP_ASSERT(glfwInit() == GLFW_TRUE, "main : Failed to initialize the glfw context");
  {
    Copium::Application application;
    Copium::Timer timer;
    int frames = 0;
    while (application.Update())
    {
      glfwPollEvents();
      if (timer.Elapsed() >= 1.0)
      {
        CP_DEBUG("main : %d fps", frames);
        frames = 0;
        timer.Start();
      }
      frames++;
    }
  }

  glfwTerminate();
  return 0;
}