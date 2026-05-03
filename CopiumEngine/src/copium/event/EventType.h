#pragma once

#include "copium/util/Enum.h"

namespace Copium
{
  CP_ENUM_CREATOR(EventType,
                  MouseMove,
                  MousePress,
                  MouseRelease,
                  MouseScroll,
                  KeyPress,
                  KeyRelease,
                  WindowResize,
                  WindowFocus,
                  ViewportResize,
                  Drop);
}
