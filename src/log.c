#include "log.h"

const char *LM_NOOP = "UNIMPLEMENTED";
const char *LM_ENTER = "ENTER FUNCTION";
const char *LM_EXIT = "EXIT FUNCTION";

const char *toMessage(enum LOG_CODES code) {
  switch (code) {
  case LC_NOOP:
    return LM_NOOP;
  case LC_ENTER:
    return LM_ENTER;
  case LC_EXIT:
    return LM_EXIT;
  default:
    return 0;
  }
}
