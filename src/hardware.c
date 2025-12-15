#include "hardware.h"

#define Z_FLAG (0b10000000)
#define N_FLAG (0b01000000)
#define H_FLAG (0b00100000)
#define C_FLAG (0b00010000)

// pass in a flag
#define SET_FLAG(value) (Regs.f |= (value))
#define CLEAR_FLAG(value) (Regs.f &= ~(value))

#define GET_Z_FLAG ((Regs.f & 0b10000000) >> 7)
#define GET_N_FLAG ((Regs.f & 0b01000000) >> 6)
#define GET_H_FLAG ((Regs.f & 0b00100000) >> 5)
#define GET_C_FLAG ((Regs.f & 0b00010000) >> 4)

uint8_t Memory[0x10000];
Registers Regs = {0};
CPU cpu = {0};
