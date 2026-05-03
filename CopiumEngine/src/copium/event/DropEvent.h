#pragma once

#include "copium/event/Event.h"

namespace Copium
{
  class DropEvent : public Event
  {
  private:
    std::vector<std::string> filePaths;

  public:
    DropEvent(const std::vector<std::string>& filePaths);

    const std::vector<std::string>& GetFilePaths() const;
  };
}
