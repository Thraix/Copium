#pragma once

#include "copium/util/Common.h"

namespace Copium
{

  class Application
  {
  public:
    Application() = default;
    virtual ~Application() = default;
    void Run();
    virtual bool Update() = 0;

  private:
    CP_DELETE_COPY_AND_MOVE_CTOR(Application);
  };
}
