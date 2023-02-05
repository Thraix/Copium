#pragma once

#include <chrono>

namespace Copium
{
  class Timer
  {
  private:
    std::chrono::time_point<std::chrono::steady_clock> startTime;
  public:
    Timer()
      : startTime{std::chrono::steady_clock::now()}
    {}

    void Start()
    {
      startTime = std::chrono::steady_clock::now();
    }

    double Elapsed()
    {
      return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
    }
  };
}
