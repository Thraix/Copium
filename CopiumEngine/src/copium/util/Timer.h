#pragma once

#include <chrono>

namespace Copium
{
  class Timer
  {
  private:
    std::chrono::time_point<std::chrono::steady_clock> startTime;

  public:
    Timer();

    void Start();
    double Elapsed();
    double ElapsedRestart();
  };
}
