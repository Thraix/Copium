#pragma once

namespace Copium
{
  class ECSManager;

  class SystemBase
  {
    friend class SystemPool;
  protected:
    ECSManager* manager;

  public:
    virtual void Run() = 0;
  };
}
