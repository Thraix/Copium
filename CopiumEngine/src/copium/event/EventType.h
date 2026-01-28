#pragma once

#include "copium/util/Enum.h"

#define CP_EVENT_TYPE_ENUMS \
  MouseMove, MousePress, MouseRelease, MouseScroll, KeyPress, KeyRelease, WindowResize, WindowFocus, ViewportResize

CP_ENUM_CREATOR(Copium, EventType, CP_EVENT_TYPE_ENUMS);
