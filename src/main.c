// main.c
#include <stdint.h>

#define WIDTH 400
#define HEIGHT 144

__attribute__((import_module("env"), import_name("print"))) extern void
print(int x);

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

uint8_t nextByte() { return Memory[Regs.pc++]; }
uint16_t nextWord() { return nextByte() | (nextByte() << 8); }

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

uint16_t *loadDest16[2][4] = {
    {&Regs.bc, &Regs.de, &Regs.hl, &Regs.sp},
    {&Regs.bc, &Regs.de, &Regs.hl, &Regs.af},
};

// the group 1 8 bit load instructions
// such as LD A, [HL] and LD A, N8
void LDG1(int byte0) {
  Instruction inst;

  if (byte0 & 0b00000100) {
    // ld x, n8
    inst.destSelect = (byte0 & 0b00110000) >> 4;

    uint8_t n8 = nextByte();
    // @TODO handle the case where the dest is [hl]
    uint8_t *dest = loadDest[inst.destNibble][inst.destSelect];

    *dest = n8;
  } else {
    if (byte0 & 0b00001000) {
      // ld a, [x]
      switch ((byte0 & 0b00110000) >> 4) {
      case 0:
        Regs.a = Memory[Regs.bc];
        break;
      case 1:
        Regs.a = Memory[Regs.de];
        break;
      case 2:
        Regs.a = Memory[Regs.hl++];
        break;
      case 3:
        Regs.a = Memory[Regs.hl--];
        break;

      default:
        break;
      }

    } else {
      // ld [x], a
      switch ((byte0 & 0b00110000) >> 4) {
      case 0:
        Memory[Regs.bc] = Regs.a;
        break;
      case 1:
        Memory[Regs.de] = Regs.a;
        break;
      case 2:
        Memory[Regs.hl++] = Regs.a;
        break;
      case 3:
        Memory[Regs.hl--] = Regs.a;
        break;

      default:
        break;
      }
    }
  }
}

void LD08(int byte0) {
  // decode instruction
  Instruction inst;

  inst.op = byte0;

  inst.mode = (byte0 & 0b11000000) >> 6;
  inst.destSelect = (byte0 & 0b00110000) >> 4;
  inst.destNibble = (byte0 & 0b00001000) >> 3;
  inst.sourceSelect = (byte0 & 0b00000111);

  if (inst.mode == 0) {
    // @TODO handle the LD [X], A family and the LD A, [X] family

    // ld X, n8 from the first 4 op rows
    uint8_t n8 = nextByte();

    // @TODO handle the case where the dest is [hl]
    uint8_t *dest = loadDest[inst.destNibble][inst.destSelect];

    *dest = n8;
  } else {
    // ld X, Y from the 2nd 4 op rows
    // @TODO handle the case where the source is [hl]
    uint8_t *source = loadSource[inst.sourceSelect];

    // @TODO handle the case where the dest is [hl]
    uint8_t *dest = loadDest[inst.destNibble][inst.destSelect];

    // execute the instruction

    *dest = *source;
  }
}

void LD16(int byte0) {
  // decode instruction
  Instruction inst;

  uint16_t n16 = nextWord();

  inst.op = byte0;

  inst.mode = (byte0 & 0b11000000) >> 6;
  inst.destSelect = (byte0 & 0b00110000) >> 4;
  inst.destNibble = (byte0 & 0b00001000) >> 3;
  inst.sourceSelect = (byte0 & 0b00000111);

  // @TODO handle the case where the dest is [hl]
  uint16_t *dest = loadDest16[inst.mode == 0b00 ? 0 : 1][inst.destSelect];

  // execute the instruction

  *dest = n16;
}

void HALT(int byte0) { return; }

// clang-format off
void (*opTable[16][16])(int byte0) = {
/* hi\lo   x0    x1    x2    x3    x4    x5    x6    x7    x8    x9    xA    xB    xC    xD    xE    xF */
/* 0x */ {NOOP, LD16, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP},
/* 1x */ {NOOP, LD16, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP},
/* 2x */ {NOOP, LD16, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP},
/* 3x */ {NOOP, LD16, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP, NOOP, NOOP, LDG1, NOOP},
/* 4x */ {LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08},
/* 5x */ {LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08},
/* 6x */ {LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08},
/* 7x */ {LD08, LD08, LD08, LD08, LD08, LD08, HALT, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08},
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

#define VRAM_TILE_DATA 0x9000
#define BLACK_TILE 0x0400 // somewhere after the program

// set up program data
#define LOW_BYTE(word) (word & 0x00FF)
#define HIGH_BYTE(word) ((word & 0xFF00) >> 8)

// compute the next frame in-place
__attribute__((export_name("boot"))) void boot(void) {
  Regs.af = 0xFFFF;
  Regs.bc = 0xFFFF;
  Regs.de = 0xFFFF;
  Regs.hl = 0xFFFF;
  Regs.sp = 0xFFFB;
  Regs.pc = 0x0000;

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

  uint16_t pc = 0;

  // ld hl, VRAM_TILE_DATA + 16
  Memory[pc++] = 0b00100001;
  Memory[pc++] = LOW_BYTE(VRAM_TILE_DATA + 16);
  Memory[pc++] = HIGH_BYTE(VRAM_TILE_DATA + 16);

  // ld de, BLACK_TILE
  Memory[pc++] = 0b00010001;
  Memory[pc++] = LOW_BYTE(BLACK_TILE);
  Memory[pc++] = HIGH_BYTE(BLACK_TILE);

  // ld b, 16
  Memory[pc++] = 0b00000110;
  Memory[pc++] = 16;

  uint8_t l0 = pc; // .loop

  // ld a, [de]
  Memory[pc++] = 0b00011010;

  // ld [hl+], a
  Memory[pc++] = 0b00100010;

  for (int i = 0; i < 64; i++) {
    // set up the tile data
    Memory[BLACK_TILE + i] = 1;
  }
}

// compute the next frame in-place
__attribute__((export_name("next_frame"))) void next_frame(void) {
  static int cycleCount = 0;

  int byte = nextByte();

  int lo = (byte & 0b00001111);
  int hi = (byte & 0b11110000) >> 4;

  opTable[hi][lo](byte);
}