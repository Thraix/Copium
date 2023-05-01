#pragma once

#include "copium/util/Enum.h"

#define CP_EVENT_RESULT_ENUMS \
  Continue, \
  Handled, \
  Focus

CP_ENUM_CREATOR(Copium, EventResult, CP_EVENT_RESULT_ENUMS);
