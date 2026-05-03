#include "copium/event/DropEvent.h"

namespace Copium
{
  DropEvent::DropEvent(const std::vector<std::string>& filePaths)
    : Event{EventType::Drop},
      filePaths{filePaths}
  {
  }

  const std::vector<std::string>& DropEvent::GetFilePaths() const
  {
    return filePaths;
  }
}
