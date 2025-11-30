#ifndef BOOT_ROM_STREAM_H_INC
#define BOOT_ROM_STREAM_H_INC

#include "../hardware.h"

#define TESTS_COUNT 7
#define TEST_GROUP_COUNT 5

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

typedef struct RegisterDiffGroup {
  int rep; // how many times to apply this test group
  int length;
  Registers expected; // an assertion to run at the end of a group
  uint16_t state[16]; // an array of pairs of (address, value) to compare
  RegisterDiff diff[256];
} RegisterDiffGroup;

// af, bc, de, hl, sp, pc
extern Registers TESTS[];
extern RegisterDiff TESTS_DIFF[];

#endif