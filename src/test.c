#include "hardware.c"
#include "hardware.h"
#include "main.c"
#include "main.h"
#include <stdio.h>

#define TESTS_COUNT 7

void print(int message) { printf("%04X\n", message); }

// af, bc, de, hl, sp, pc
Registers TESTS[TESTS_COUNT] = {
    {.af = 0x0000,
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x0000,
     .sp = 0x0000,
     .pc = 0x0000},
    {.af = 0x0000,
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x0000,
     .sp = 0xFFFE,
     .pc = 0x0003},
    {.af = 0x0080,
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x0000,
     .sp = 0xFFFE,
     .pc = 0x0004},
    {.af = 0x0080,
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x9FFF,
     .sp = 0xFFFE,
     .pc = 0x0007},
    {.af = 0x0080,
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x9FFE,
     .sp = 0xFFFE,
     .pc = 0x0008},
    {.af = 0x0020,
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x9FFE,
     .sp = 0xFFFE,
     .pc = 0x000A},
    {.af = 0x0020,
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x9FFE,
     .sp = 0xFFFE,
     .pc = 0x0007},
};

int runTest(Registers regs) {
  return regs.af == Regs.af && regs.bc == Regs.bc && regs.de == Regs.de &&
         regs.hl == Regs.hl && regs.sp == Regs.sp && regs.pc == Regs.pc;
}

int runTests() {
  int i = 0;
  int fail = 0;

  while (i < TESTS_COUNT) {
    Registers regs = TESTS[i];

    if (!runTest(regs)) {
      printf("fail test %d\n", i);
      printf("  expected %04X %04X %04X %04X %04X %04X\n", regs.af, regs.bc,
             regs.de, regs.hl, regs.sp, regs.pc);
      printf("  received %04X %04X %04X %04X %04X %04X\n", Regs.af, Regs.bc,
             Regs.de, Regs.hl, Regs.sp, Regs.pc);
      fail = 1;
      break;
    }

    i++;
    next_instruction();
  }

  if (fail) {
    printf("passed %d tests before failing\n", i);
  } else {
    printf("all tests passing");
  }

  return 0;
}

int main() {
  boot();

  runTests();

  return 1;
}