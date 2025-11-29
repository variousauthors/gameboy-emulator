#ifndef BOOT_ROM_STREAM_H_INC
#define BOOT_ROM_STREAM_H_INC

#include "../hardware.h"

#define TESTS_COUNT 7

// needs to be able to encode +/- 0xFFFF
// so fields are signed 32 bit, which is a little
// wasteful lol, but this is tests
typedef struct RegisterDiff {
  int32_t af;
  int32_t bc;
  int32_t de;
  int32_t hl;
  int32_t sp;
  int32_t pc;
} RegisterDiff;

// af, bc, de, hl, sp, pc
extern Registers TESTS[];
extern RegisterDiff TESTS_DIFF[];

#endif