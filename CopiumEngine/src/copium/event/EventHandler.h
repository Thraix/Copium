#pragma once

#include "copium/event/Event.h"
#include "copium/event/EventResult.h"

namespace Copium
{
  class EventHandler
  {
  public:
    virtual EventResult OnEvent(const Event& event) = 0;
  };
}