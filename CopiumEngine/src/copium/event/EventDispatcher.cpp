#include "copium/event/EventDispatcher.h"

#include "copium/util/Common.h"

namespace Copium
{
  EventHandler* EventDispatcher::focusedEventHandler = nullptr;
  std::vector<EventHandler*> EventDispatcher::eventHandlers;
  std::queue<std::unique_ptr<Event>> EventDispatcher::events;

  void EventDispatcher::Dispatch()
  {
    while (!events.empty())
    {
      std::unique_ptr<Event> event = std::move(events.front());
      events.pop();
      if (focusedEventHandler)
      {
        EventResult result = focusedEventHandler->OnEvent(*event);
        if (result == EventResult::Handled || result == EventResult::Focus)
          return;
      }
      for (auto& eventHandler : eventHandlers)
      {
        if (eventHandler == focusedEventHandler)
          continue;
        EventResult result = eventHandler->OnEvent(*event);
        switch (result)
        {
        case EventResult::Continue:
          continue;
        case EventResult::Handled:
          return;
        case EventResult::Focus:
          focusedEventHandler = eventHandler;
          return;
        default:
          CP_ABORT("Unreachable switch case: %s", ToString(result).c_str());
        }
      }
    }
  }

  void EventDispatcher::AddEventHandler(EventHandler* eventHandler)
  {
    eventHandlers.emplace_back(eventHandler);
  }

  void EventDispatcher::RemoveEventHandler(EventHandler* eventHandler)
  {
    for (auto it = eventHandlers.begin(); it != eventHandlers.end(); ++it)
    {
      if (*it == eventHandler)
      {
        eventHandlers.erase(it);
        return;
      }
    }
  }
}
