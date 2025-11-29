#include "log.h"
#include "main.c"
#include "main.h"
#include "tests/boot-rom-stream.c"
#include "tests/boot-rom-stream.h"
#include <stdio.h>

// clang-format off
char DISASSEMBLY_LOOKUP[16][16][16] = {
  {"NOP\0", "LD BC, n16\0", "LD [BC], A\0", "INC BC\0", "INC B\0", "DEC B\0", "LD B, n8\0", "RLCA\0", "LD [a16], SP\0", "ADD HL, BC\0", "LD A, [BC]\0", "DEC BC\0", "INC C\0", "DEC C\0", "LD C, n8\0", "RRCA\0"},
  {"STOP n8\0", "LD DE, n16\0", "LD [DE], A\0", "INC DE\0", "INC D\0", "DEC D\0", "LD D, n8\0", "RLA\0", "JR e8\0", "ADD HL, DE\0", "LD A, [DE]\0", "DEC DE\0", "INC E\0", "DEC E\0", "LD E, n8\0", "RRA\0"},
  {"JR NZ, e8\0", "LD HL, n16\0", "LD [HL+], A\0", "INC HL\0", "INC H\0", "DEC H\0", "LD H, n8\0", "DAA\0", "JR Z, e8\0", "ADD HL, HL\0", "LD A, [HL+]\0", "DEC HL\0", "INC L\0", "DEC L\0", "LD L, n8\0", "CPL\0"},
  {"JR NC, e8\0", "LD SP, n16\0", "LD [HL-], A\0", "INC SP\0", "INC [HL]\0", "DEC [HL]\0", "LD [HL], n8\0", "SCF\0", "JR C, e8\0", "ADD HL, SP\0", "LD A, [HL-]\0", "DEC SP\0", "INC A\0", "DEC A\0", "LD A, n8\0", "CCF\0"},
  {"LD B, B\0", "LD B, C\0", "LD B, D\0", "LD B, E\0", "LD B, H\0", "LD B, L\0", "LD B, [HL]\0", "LD B, A\0", "LD C, B\0", "LD C, C\0", "LD C, D\0", "LD C, E\0", "LD C, H\0", "LD C, L\0", "LD C, [HL]\0", "LD C, A\0"},
  {"LD D, B\0", "LD D, C\0", "LD D, D\0", "LD D, E\0", "LD D, H\0", "LD D, L\0", "LD D, [HL]\0", "LD D, A\0", "LD E, B\0", "LD E, C\0", "LD E, D\0", "LD E, E\0", "LD E, H\0", "LD E, L\0", "LD E, [HL]\0", "LD E, A\0"},
  {"LD H, B\0", "LD H, C\0", "LD H, D\0", "LD H, E\0", "LD H, H\0", "LD H, L\0", "LD H, [HL]\0", "LD H, A\0", "LD L, B\0", "LD L, C\0", "LD L, D\0", "LD L, E\0", "LD L, H\0", "LD L, L\0", "LD L, [HL]\0", "LD L, A\0"},
  {"LD [HL], B\0", "LD [HL], C\0", "LD [HL], D\0", "LD [HL], E\0", "LD [HL], H\0", "LD [HL], L\0", "HALT\0", "LD [HL], A\0", "LD A, B\0", "LD A, C\0", "LD A, D\0", "LD A, E\0", "LD A, H\0", "LD A, L\0", "LD A, [HL]\0", "LD A, A\0"},
  {"ADD A, B\0", "ADD A, C\0", "ADD A, D\0", "ADD A, E\0", "ADD A, H\0", "ADD A, L\0", "ADD A, [HL]\0", "ADD A, A\0", "ADC A, B\0", "ADC A, C\0", "ADC A, D\0", "ADC A, E\0", "ADC A, H\0", "ADC A, L\0", "ADC A, [HL]\0", "ADC A, A\0"},
  {"SUB A, B\0", "SUB A, C\0", "SUB A, D\0", "SUB A, E\0", "SUB A, H\0", "SUB A, L\0", "SUB A, [HL]\0", "SUB A, A\0", "SBC A, B\0", "SBC A, C\0", "SBC A, D\0", "SBC A, E\0", "SBC A, H\0", "SBC A, L\0", "SBC A, [HL]\0", "SBC A, A\0"},
  {"AND A, B\0", "AND A, C\0", "AND A, D\0", "AND A, E\0", "AND A, H\0", "AND A, L\0", "AND A, [HL]\0", "AND A, A\0", "XOR A, B\0", "XOR A, C\0", "XOR A, D\0", "XOR A, E\0", "XOR A, H\0", "XOR A, L\0", "XOR A, [HL]\0", "XOR A, A\0"},
  {"OR A, B\0", "OR A, C\0", "OR A, D\0", "OR A, E\0", "OR A, H\0", "OR A, L\0", "OR A, [HL]\0", "OR A, A\0", "CP A, B\0", "CP A, C\0", "CP A, D\0", "CP A, E\0", "CP A, H\0", "CP A, L\0", "CP A, [HL]\0", "CP A, A\0"},
  {"RET NZ\0", "POP BC\0", "JP NZ, a16\0", "JP a16\0", "CALL NZ, a16\0", "PUSH BC\0", "ADD A, n8\0", "RST $00\0", "RET Z\0", "RET\0", "JP Z, a16\0", "PREFIX\0", "CALL Z, a16\0", "CALL a16\0", "ADC A, n8\0", "RST $08\0"},
  {"RET NC\0", "POP DE\0", "JP NC, a16\0", " - \0", "CALL NC, a16\0", "PUSH DE\0", "SUB A, n8\0", "RST $10\0", "RET C\0", "RETI\0", "JP C, a16\0", " - \0", "CALL C, a16\0", " - \0", "SBC A, n8\0", "RST $18\0"},
  {"LDH [a8], A\0", "POP HL\0", "LDH [C], A\0", " - \0", " - \0", "PUSH HL\0", "AND A, n8\0", "RST $20\0", "ADD SP, e8\0", "JP HL\0", "LD [a16], A\0", " - \0", " - \0", " - \0", "XOR A, n8\0", "RST $28\0"},
  {"LDH A, [a8]\0", "POP AF\0", "LDH A, [C]\0", "DI\0", " - \0", "PUSH AF\0", "OR A, n8\0", "RST $30\0", "LD HL, SP + e8\0", "LD SP, HL\0", "LD A, [a16]\0", "EI\0", " - \0", " - \0", "CP A, n8\0", "RST $38\0"},
};
// clang-format on

void print(enum LOG_CODES code) {
  const char *message = toMessage(code);

  if (message) {
    printf("%s\n", message);
  } else {
    printf("byte %04x\n", (uint8_t)code);
  }
}

uint8_t (*nextByte)(void);
uint16_t (*nextWord)(void);
uint8_t (*getByte)(uint16_t address);
uint8_t (*getWord)(uint16_t address);

int assertEqual(Registers a, Registers b) {
  return a.af == b.af && a.bc == b.bc && a.de == b.de && a.hl == b.hl &&
         a.sp == b.sp && a.pc == b.pc;
}

void applyDiff(Registers *regs, RegisterDiff diff) {
  regs->af += diff.af;
  regs->bc += diff.bc;
  regs->de += diff.de;
  regs->hl += diff.hl;
  regs->pc += diff.pc;
  regs->sp += diff.sp;
}

int runTests() {
  int i = 0;
  int fail = 0;

  Registers regs = {0};

  while (i < TEST_GROUP_COUNT) {
    int j = 0;

    while (j < TEST_GROUPS[i].rep) {
      int k = 0;

      while (k < TEST_GROUPS[i].length) {
        applyDiff(&regs, TEST_GROUPS[i].diff[k]);

        // store the PC before we run, for debugging
        getByte = get_Byte();
        uint16_t address = Regs.pc;

        next_instruction();

        if (!assertEqual(regs, Regs)) {
          uint8_t opcode = getByte(address);
          int lo = (opcode & 0b00001111);
          int hi = (opcode & 0b11110000) >> 4;

          printf("failure in test group %d\n", i);
          printf("failing test %d\n", k);
          printf("during rep %d\n", j);
          printf("  address: %02X\n", address);
          printf("  opcode: %02X\n", opcode);
          printf("  instruction: %s\n", DISASSEMBLY_LOOKUP[hi][lo]);
          printf("  expected %04X %04X %04X %04X %04X %04X\n", regs.af, regs.bc,
                 regs.de, regs.hl, regs.sp, regs.pc);
          printf("  received %04X %04X %04X %04X %04X %04X\n", Regs.af, Regs.bc,
                 Regs.de, Regs.hl, Regs.sp, Regs.pc);
          fail = 1;
          break;
        }
        k++;
      }
      j++;
    }

    // finally after each group, assert we
    // are in the expected CPU state after the group
    if (!fail && !assertEqual(TEST_GROUPS[i].expected, Regs)) {
      Registers expected = TEST_GROUPS[i].expected;

      printf("failed sanity check for test group %d\n", i);
      printf("  expected %04X %04X %04X %04X %04X %04X\n", expected.af,
             expected.bc, expected.de, expected.hl, expected.sp, expected.pc);
      printf("  received %04X %04X %04X %04X %04X %04X\n", Regs.af, Regs.bc,
             Regs.de, Regs.hl, Regs.sp, Regs.pc);
      fail = 1;
      break;
    }

    i++;
  }

  if (fail) {
    printf("passed %d test groups before failing\n", i - 2);
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