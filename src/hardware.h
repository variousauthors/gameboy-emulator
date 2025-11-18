#ifndef HARDWARE_H_INC
#define HARDWARE_H_INC

#include <stdint.h>

typedef struct CPU {
  unsigned halt : 1;
  unsigned boot : 1;
} CPU;

typedef struct Registers {
  union {
    struct {
      uint8_t f; // lo bits
      uint8_t a; // hi bits
    };
    uint16_t af;
  };
  union {
    struct {
      uint8_t c; // lo bits
      uint8_t b; // hi bits
    };
    uint16_t bc;
  };
  union {
    struct {
      uint8_t e; // lo bits
      uint8_t d; // hi bits
    };
    uint16_t de;
  };
  union {
    struct {
      uint8_t l; // lo bits
      uint8_t h; // hi bits
    };
    uint16_t hl;
  };

  uint16_t sp;
  uint16_t pc;

  // stuff we use
  unsigned halt : 1;
} Registers;

extern uint8_t Memory[0x10000];
extern Registers Regs;
extern CPU cpu;

#endif