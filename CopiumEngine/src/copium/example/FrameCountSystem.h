#pragma once

#include "copium/ecs/System.h"
#include "copium/example/Components.h"

namespace Copium
{
  class FrameCountSystem : public System<FrameCountC, TextC>
  {
  private:
    Timer timer;
    int frameCounter = 0;
    int fps = 0;

  public:
    void RunEntity(Entity entity, FrameCountC& frameCount, TextC& text)
    {
      text.text = std::to_string(fps) + " fps";
    }

    void Run() override
    {
      if (timer.Elapsed() >= 1.0)
      {
        fps = frameCounter;
        frameCounter = 0;
        timer.Start(); // Not quite accurate since the elapsed time might me 1.1, then we lose 0.1 precision
        System::Run();
      }
      frameCounter++;
    }
  };

}