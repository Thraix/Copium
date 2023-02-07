#include "Timer.h"

namespace Copium
{
  Timer::Timer()
    : startTime{std::chrono::steady_clock::now()}
  {}

  void Timer::Start()
  {
    startTime = std::chrono::steady_clock::now();
  }

  double Timer::Elapsed()
  {
    return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - startTime).count();
  }
}
