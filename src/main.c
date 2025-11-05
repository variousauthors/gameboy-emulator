// main.c
#include <stdint.h>

#define WIDTH 160
#define HEIGHT 144

__attribute__((import_module("env"), import_name("print"))) extern void
print(int x);

typedef struct CPU {
  unsigned halt : 1;

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

// framebuffer in linear memory
uint32_t framebuffer[WIDTH * HEIGHT];

uint8_t Memory[0x10000];
Registers Regs = {0};
CPU cpu = {0};

uint8_t nextByte() { return Memory[Regs.pc++]; }
uint16_t nextWord() { return nextByte() | (nextByte() << 8); }

// a simple animation counter
static unsigned frame = 0;

void NOOP(int byte0) { return; }

typedef struct Instruction {
  // the first byte
  uint8_t op;

  unsigned block : 2;
  unsigned dest : 3;
  unsigned source : 3;

} Instruction;

uint8_t *loadSource[8] = {
    &Regs.b, &Regs.c, &Regs.d, &Regs.e, &Regs.h, &Regs.l, 0, &Regs.a,
};

// @TODO replace this array with a function so that we can do
// i == 7 ? Memory[Regs.hl] : r16[i]
// because right now nothing handles the [hl] forms
uint8_t *r8[8] = {
    &Regs.b, &Regs.c, &Regs.d, &Regs.e, &Regs.h, &Regs.l, 0, &Regs.a,
};

uint16_t *r16[4] = {
    &Regs.bc,
    &Regs.de,
    &Regs.hl,
    &Regs.sp,
};

uint8_t *loadDest[2][4] = {
    {&Regs.b, &Regs.d, &Regs.h, 0},
    {&Regs.c, &Regs.e, &Regs.l, &Regs.a},
};

uint16_t *loadDest16[2][4] = {
    {&Regs.bc, &Regs.de, &Regs.hl, &Regs.sp},
    {&Regs.bc, &Regs.de, &Regs.hl, &Regs.af},
};

void HALT(int byte0) { cpu.halt = 1; }

void LD08(int byte0) {
  // decode instruction
  Instruction inst;

  inst.op = byte0;

  inst.block = (byte0 & 0b11000000) >> 6;
  inst.dest = (byte0 & 0b00111000) >> 3;
  inst.source = (byte0 & 0b00000111);

  if (inst.block == 0) {
    if (inst.source == 0b010) {
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

    } else {
      // ld X, n8 from the first 4 op rows
      uint8_t n8 = nextByte();

      // @TODO handle the case where the dest is [hl]
      uint8_t *dest = r8[inst.dest];

      *dest = n8;
    }
  } else {
    // ld X, Y from the 2nd 4 op rows
    // @TODO handle the case where the source is [hl]
    uint8_t *source = r8[inst.source];

    // @TODO handle the case where the dest is [hl]
    uint8_t *dest = r8[inst.dest];

    // execute the instruction

    *dest = *source;
  }
}

void LD16(int byte0) {
  // decode instruction
  Instruction inst;

  uint16_t n16 = nextWord();

  inst.op = byte0;

  inst.block = (byte0 & 0b11000000) >> 6;
  inst.dest = (byte0 & 0b00110000) >> 4;

  // @TODO handle the case where the dest is [hl]
  uint16_t *dest = r16[inst.dest];

  // execute the instruction

  *dest = n16;
}

// inc/dec for r16
void ST16(int byte0) {
  uint16_t *operand = r16[(byte0 & 0b00110000) >> 4];

  if (byte0 & 0b00001000) {
    (*operand)--;
  } else {
    (*operand)++;
  }
}

// inc/dec for r8
void ST08(int byte0) {
  uint8_t *operand = r8[(byte0 & 0b00111000) >> 3];

  if (byte0 & 0b00000001) {
    (*operand)--;
    Regs.f |= 0b01000000;
  } else {
    (*operand)++;
    Regs.f &= ~0b01000000;
  }

  if (*operand) {
    // set z to 0
    Regs.f &= ~0b10000000;
  } else {
    // set z to 1
    Regs.f |= 0b10000000;
  }
}

#define Z_FLAG ((Regs.f & 0b10000000) >> 7)
#define N_FLAG ((Regs.f & 0b01000000) >> 6)
#define H_FLAG ((Regs.f & 0b00100000) >> 5)
#define C_FLAG ((Regs.f & 0b00010000) >> 4)

uint8_t cond(uint8_t value) {
  switch (value) {
  case 0:
    return !Z_FLAG;
  case 1:
    return Z_FLAG;
  case 2:
    return !C_FLAG;
  case 3:
    return C_FLAG;
  default:
    return 0;
  }
}

void JR08(int byte0) {
  if ((byte0 & 0b00100000) && !cond((byte0 & 0b00011000) >> 3)) {
    return;
  }

  uint8_t dest = nextByte();

  Regs.pc = dest;
}

void JP16(int byte0) {
  if ((byte0 & 0b00000001) && !cond((byte0 & 0b00011000) >> 3)) {
    return;
  }

  if (byte0 & 0b0010000) {
    print(1);
    Regs.pc = Memory[Regs.hl];
  } else {
    print(2);
    Regs.pc = nextWord();
  }
}

// clang-format off
void (*opTable[16][16])(int byte0) = {
/* hi\lo   x0    x1    x2    x3    x4    x5    x6    x7    x8    x9    xA    xB    xC    xD    xE    xF */
/* 0x */ {NOOP, LD16, LD08, ST16, ST08, ST08, LD08, NOOP, NOOP, NOOP, LD08, ST16, ST08, ST08, LD08, NOOP},
/* 1x */ {NOOP, LD16, LD08, ST16, ST08, ST08, LD08, NOOP, JR08, NOOP, LD08, ST16, ST08, ST08, LD08, NOOP},
/* 2x */ {JR08, LD16, LD08, ST16, ST08, ST08, LD08, NOOP, JR08, NOOP, LD08, ST16, ST08, ST08, LD08, NOOP},
/* 3x */ {JR08, LD16, LD08, ST16, ST08, ST08, LD08, NOOP, JR08, NOOP, LD08, ST16, ST08, ST08, LD08, NOOP},
/* 4x */ {LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08},
/* 5x */ {LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08},
/* 6x */ {LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08},
/* 7x */ {LD08, LD08, LD08, LD08, LD08, LD08, HALT, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08, LD08},
/* 8x */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* 9x */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Ax */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Bx */ {NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
/* Cx */ {NOOP, NOOP, NOOP, JP16, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP, NOOP},
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

#define ROM0_START 0

#define VRAM_TILE_DATA_BLOCK_2 0x9000
#define VRAM_TILE_DATA_BLOCK_SIZE 0x800
#define VRAM_MAP_DATA_9800 0x9800
#define VRAM_MAP_DATA_SIZE 0x400

#define BIT(byte, n) (((byte) >> (n)) & 1)

uint32_t GRAYS[4] = {0xFFFFFFFF, 0xFFAAAAAA, 0xFF444444, 0xFF000000};

#define DRAW_WIDTH (20)
#define DRAW_HEIGHT (144)

void draw() {
  for (int y = 0; y < DRAW_HEIGHT; y++) {
    for (int x = 0; x < DRAW_WIDTH; x++) {
      uint8_t tileX = x;
      uint8_t tileY = y / 8;

      // while tile in the tile db
      uint8_t tileIndex = Memory[VRAM_MAP_DATA_9800 + tileY * 32 + tileX] * 16;

      // which bytes to read from the 8x8 tile
      uint8_t py = 2 * (y % 8);

      uint8_t lowByte = Memory[VRAM_TILE_DATA_BLOCK_2 + tileIndex + py];
      uint8_t highByte = Memory[VRAM_TILE_DATA_BLOCK_2 + tileIndex + py + 1];

      for (int i = 0; i < 8; i++) {
        uint8_t index = (BIT(lowByte, 7 - i) << 1) | (BIT(highByte, 7 - i));
        uint32_t grey = GRAYS[index];
        // push 8 pixels
        framebuffer[y * 160 + x * 8 + i] = grey;
      }
    }
  }
}

#define BLACK_TILE 0x0400 // somewhere after the program

// set up program data
#define LOW_BYTE(word) (word & 0x00FF)
#define HIGH_BYTE(word) ((word & 0xFF00) >> 8)

#define ROTL8(x, n) (uint8_t)(((x) << (n)) | ((x) >> (8 - (n))))

// compute the next frame in-place
__attribute__((export_name("boot"))) void boot(void) {
  Regs.af = 0xFFFF;
  Regs.bc = 0xFFFF;
  Regs.de = 0xFFFF;
  Regs.hl = 0xFFFF;
  Regs.sp = 0xFFFB;
  Regs.pc = 0x0100;

  /*
  program:
  ld hl, VRAM_TILE_DATA_BLOCK_2 + 16
  ld de, BLACK_TILE
  ld b, 16

  .loop
    ld a, [de]
    ld [hl+], a
    inc de

    dec b
    jp nz, .loop

  data
    64 bytes all 1
  */

  /*
  uint16_t pc = 0;

  // ld hl, VRAM_TILE_DATA_BLOCK_2 + 16
  Memory[pc++] = 0b00100001;
  Memory[pc++] = LOW_BYTE(VRAM_TILE_DATA_BLOCK_2 + 16);
  Memory[pc++] = HIGH_BYTE(VRAM_TILE_DATA_BLOCK_2);

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

  // inc de
  Memory[pc++] = 0b00010011;

  // dec b
  Memory[pc++] = 0b00000101;

  // jr nz, loop
  Memory[pc++] = 0b00100000;
  Memory[pc++] = l0;

  // halt
  Memory[pc++] = 0b01110110;

  uint8_t bits = 0b10101010;

  // fill a black tile graphic in VRAM
  for (int i = 0; i < 16; i += 2) {
    // set up the tile data
    Memory[BLACK_TILE + i] = bits;
    Memory[BLACK_TILE + i + 1] = bits;
    bits = ROTL8(bits, 1);
  }

  // fill the tile map with a grid
  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 32; x++) {
      if (y % 2 == 0 && x % 2 == 0) {
        Memory[VRAM_MAP_DATA_9800 + y * 32 + x] = 1;
      }

      if (y % 2 == 1 && x % 2 == 1) {
        Memory[VRAM_MAP_DATA_9800 + y * 32 + x] = 1;
      }
    }
  }
  */
}

// timing constants
#define DOTS_PER_FRAME 70244
#define CYCLES_PER_FRAME (DOTS_PER_FRAME >> 2)

// compute the next frame in-place
__attribute__((export_name("next_frame"))) void next_frame(void) {
  print(99);
  return;

  static int cycleCount = 0;

  // for now I'm just going to ham-fist the
  // drawing and hope it performs
  // @TODO implement this like the PPU
  draw();

  while (cycleCount < CYCLES_PER_FRAME) {
    cycleCount++;
    if (cpu.halt) {
      continue;
    }

    int byte = nextByte();
    print(byte);

    int lo = (byte & 0b00001111);
    int hi = (byte & 0b11110000) >> 4;

    opTable[hi][lo](byte);
  }
}