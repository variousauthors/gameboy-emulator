// main.c
#include <stdint.h>

#define WIDTH 400
#define HEIGHT 144

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
} Registers;

// framebuffer in linear memory
uint32_t framebuffer[WIDTH * HEIGHT];

uint8_t Memory[0xFFFF];
Registers Regs = {0};

// a simple animation counter
static unsigned frame = 0;

void NOOP(int byte0) { return; }

typedef struct Instruction {
  // the first byte
  uint8_t op;

  unsigned mode : 2;         // selects a group of rows from the op table
  unsigned destSelect : 2;   // select one of the 8-bit registers
  unsigned destNibble : 1;   // 0 -> hi | 1 -> lo
  unsigned sourceSelect : 3; // select which register to use

} Instruction;

uint8_t *loadSource[8] = {
    &Regs.b, &Regs.c, &Regs.d, &Regs.e, &Regs.h, &Regs.l, 0, &Regs.a,
};

uint8_t *loadDest[2][4] = {
    {&Regs.b, &Regs.d, &Regs.h, 0},
    {&Regs.c, &Regs.e, &Regs.l, &Regs.a},
};

void LOAD(int byte0) {
  // decode instruction
  Instruction inst;

  inst.op = byte0;

  inst.destSelect = (byte0 & 0b00110000) >> 4;
  inst.destNibble = (byte0 & 0b00001000) >> 3;
  inst.sourceSelect = (byte0 & 0b00000111);

  // @TODO handle the case where the source is [hl]
  uint8_t *source = loadSource[inst.sourceSelect];

  // @TODO handle the case where the dest is [hl]
  uint8_t *dest = loadDest[inst.destNibble][inst.destSelect];

  // execute the instruction

  *dest = *source;
}

void HALT(int byte0) { return; }

// clang-format off
void (*opTable[16][16])(int byte0) = {
/* hi\lo   x0    x1    x2    x3    x4    x5    x6    x7    x8    x9    xA    xB    xC    xD    xE    xF */
/* 0x */ {NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP},
/* 1x */ {NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP},
/* 2x */ {NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP},
/* 3x */ {NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP, NOOP, NOOP, LOAD, NOOP},
/* 4x */ {LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD},
/* 5x */ {LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD},
/* 6x */ {LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD},
/* 7x */ {LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, HALT, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD, LOAD},
/* 8x */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* 9x */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Ax */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Bx */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Cx */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Fx */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Ex */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Fx */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
};
// clang-format on

// export framebuffer address so JS can read it
__attribute__((export_name("get_framebuffer"))) uint32_t *
get_framebuffer(void) {
  return framebuffer;
}

// export framebuffer address so JS can read it
__attribute__((export_name("get_memory"))) uint8_t *get_memory(void) {
  return Memory;
}

// export framebuffer address so JS can read it
__attribute__((export_name("get_registers"))) Registers *get_registers(void) {
  return &Regs;
}

// export constants so JS can read them too
__attribute__((export_name("get_width"))) int get_width(void) { return WIDTH; }

__attribute__((export_name("get_height"))) int get_height(void) {
  return HEIGHT;
}

#define CYCLE 1
#define DOT (4 * CYCLE)
#define SCANLINE (456 * DOT)
#define FRAME (154 * SCANLINE)

#define ROM0_START 0

// compute the next frame in-place
__attribute__((export_name("boot"))) void boot(void) {
  Regs.af = 0xFFFF;
  Regs.bc = 0xFFFF;
  Regs.de = 0xFFFF;
  Regs.hl = 0xFFFF;
  Regs.sp = 0xFFFB;
  Regs.pc = 0x0000;

#define VRAM_TILE_DATA 0x9000
#define BLACK_TILE 0x0400 // somewhere after the program

  /*
  program:
  ld hl, VRAM_TILE_DATA + 16
  ld de, BLACK_TILE
  ld b, 16

  .loop
    ld a, [de]
    ld [hl], a
    inc de
    inc hl

    dec b
    jp nz, .loop

  data
    64 bytes all 1
  */

  for (int i = 0; i < 64; i++) {
    // set up the tile data
    Memory[BLACK_TILE + i] = 1;
  }
}

// compute the next frame in-place
__attribute__((export_name("next_frame"))) void next_frame(void) {
  static int cycleCount = 0;

  Regs.a = 7;

  int byte = 0b01000111; // ld b, a

  int lo = (byte & 0b00001111);
  int hi = (byte & 0b11110000) >> 4;

  opTable[hi][lo](byte);
}