#ifndef LOG_H_INC
#define LOG_H_INC

enum LOG_CODES { LC_NOOP = 99 };

extern const char *LM_NOOP;

const char *toMessage(enum LOG_CODES code);

#endif