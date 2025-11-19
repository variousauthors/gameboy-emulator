#include "log.h"

const char *LM_NOOP = "NOOP";

const char *toMessage(enum LOG_CODES code) {
  switch (code) {
  case LC_NOOP:
    return LM_NOOP;
  default:
    return 0;
  }
}
