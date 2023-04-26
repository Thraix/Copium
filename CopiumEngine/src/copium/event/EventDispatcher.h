#pragma once

#include "copium/event/Event.h"
#include "copium/event/EventHandler.h"

#include <memory>
#include <vector>
#include <queue>

namespace Copium
{
  class EventDispatcher
  {
  private:
    static EventHandler* focusedEventHandler;
    static std::vector<EventHandler*> eventHandlers;
    static std::queue<std::unique_ptr<Event>> events;
  public:
    template <typename EventType>
    static void QueueEvent(const EventType& event)
    {
      events.push(std::make_unique<EventType>(event));
    }
    static void Dispatch();
    static void AddEventHandler(EventHandler* eventHandler);
    static void RemoveEventHandler(EventHandler* eventHandler);
  };
}