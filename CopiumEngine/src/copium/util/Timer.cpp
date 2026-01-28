#include "copium/util/Timer.h"

namespace Copium
{
  Timer::Timer()
    : startTime{std::chrono::steady_clock::now()}
  {
  }

  void Timer::Start()
  {
    startTime = std::chrono::steady_clock::now();
  }

  double Timer::Elapsed()
  {
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count();
  }

  double Timer::ElapsedRestart()
  {
    std::chrono::time_point<std::chrono::steady_clock> newTime = std::chrono::steady_clock::now();

    double elapsedTime = std::chrono::duration<double>(newTime - startTime).count();
    startTime = newTime;

    return elapsedTime;
  }
}
