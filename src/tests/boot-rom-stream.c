
#include "boot-rom-stream.h"

/* we need to fake a ROM header for the boot-rom to read */
uint8_t ENTRY_POINT_ROM[4] = {
    0x00,
    0xC3,
    0x01,
    0x50,
};

uint8_t NINTENDO_LOGO_ROM[0x30] = {
    0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83,
    0x00, 0x0C, 0x00, 0x0D, 0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E,
    0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99, 0xBB, 0xBB, 0x67, 0x63,
    0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E};

// do any init we need for this test
void initBootROMStream(uint8_t *memory) {
  int base = 0x0100;

  for (int i = 0; i < 4; i++) {
    memory[base + i] = ENTRY_POINT_ROM[i];
  }

  base = 0x0104;

  for (int i = 0; i < 0x30; i++) {
    memory[base + i] = NINTENDO_LOGO_ROM[i];
  }
}

RegisterDiffGroup
    TEST_GROUPS[TEST_GROUP_COUNT] =
        {{
             .rep = 1,
             .length = 3,
             .expected =
                 {
                     .af = 0x0080,
                     .bc = 0x0000,
                     .de = 0x0000,
                     .hl = 0x9FFF,
                     .sp = 0xFFFE,
                     .pc = 0x0007,
                 },
             .state =
                 {
                     0x0000,
                     0x0000,
                 },
             .diff = {{.af = 0x0000, // ld sp, 0xFFFE
                       .bc = 0x0000,
                       .de = 0x0000,
                       .hl = 0x0000,
                       .sp = 0xFFFE,  // + 0xFFFE
                       .pc = 0x0003}, // + 3
                      {.af = 0x0080,  // xor a, a
                       .bc = 0x0000,
                       .de = 0x0000,
                       .hl = 0x0000,
                       .sp = 0x0000,
                       .pc = 0x0001},
                      {.af = 0x0000, // ld hl, 0x9FFF
                       .bc = 0x0000,
                       .de = 0x0000,
                       .hl = 0x9FFF,
                       .sp = 0x0000,
                       .pc = 0x0003}},
         },
         {.rep = 1,
          .length = 3,
          .expected =
              {
                  .af = 0x0020,
                  .bc = 0x0000,
                  .de = 0x0000,
                  .hl = 0x9FFE,
                  .sp = 0xFFFE,
                  .pc = 0x0007,
              },
          .state =
              {
                  0x9FFF,
                  0x0000,
              },
          .diff = {{.af = 0x0000, // ld[hl-], a
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = -0x0001,
                    .sp = 0x0000,
                    .pc = 0x0001},
                   {.af = -0x0060, // bit 7, h (prefix)
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = 0x0000,
                    .sp = 0x0000,
                    .pc = 0x0002},
                   {.af = 0x0000, // jr nz, n8
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = 0x0000,
                    .sp = 0x0000,
                    .pc = -0x0003}}},
         {.rep = 8190,
          .length = 3,
          .expected =
              {
                  .af = 0x0020,
                  .bc = 0x0000,
                  .de = 0x0000,
                  .hl = 0x8000,
                  .sp = 0xFFFE,
                  .pc = 0x0007,
              },
          .state =
              {
                  0x8000,
                  0x0000,
              },
          .diff = {{.af = 0x0000, // ld[hl-], a
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = -0x0001,
                    .sp = 0x0000,
                    .pc = 0x0001},
                   {.af = 0x0000, // bit 7, h (prefix)
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = 0x0000,
                    .sp = 0x0000,
                    .pc = 0x0002},
                   {.af = 0x0000, // jr nz, n8
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = 0x0000,
                    .sp = 0x0000,
                    .pc = -0x0003}}},
         {.rep = 1,
          .length = 3,
          .expected =
              {
                  .af = 0x00A0,
                  .bc = 0x0000,
                  .de = 0x0000,
                  .hl = 0x7FFF,
                  .sp = 0xFFFE,
                  .pc = 0x000C,
              },
          .state =
              {
                  0x0000,
                  0x0000,
              },
          .diff = {{.af = 0x0000, // ld [hl-], a
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = -0x0001,
                    .sp = 0x0000,
                    .pc = 0x0001},
                   {.af = 0x0080, // bit 7, h (prefix)
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = 0x0000,
                    .sp = 0x0000,
                    .pc = 0x0002},
                   {.af = 0x0000, // jr nz, n8
                    .bc = 0x0000,
                    .de = 0x0000,
                    .hl = 0x0000,
                    .sp = 0x0000,
                    .pc = 0x0002}}},
         {.rep = 1,
          .length = 16,
          .expected =
              {
                  .af = 0xCE00,
                  .bc = 0x0012,
                  .de = 0x0104,
                  .hl = 0x8010,
                  .sp = 0xFFFE,
                  .pc = 0x0028,
              },
          .state =
              {
                  0xFF11, // ldh [c], a
                  0x0080,
                  0xFF12, // ldh [c], a
                  0x00F3,
                  0xFF25, // ldd [hl], a
                  0x00F3,
                  0xFF24, // ld [hl], a
                  0x0077,
                  0xFF47, // ldh [rBGP], a
                  0x00FC,
              },
          .diff =
              {
                  {.af = 0x0000, // ld hl, n16
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = -0x80D9,
                   .sp = 0x0000,
                   .pc = 0x0003},
                  {.af = 0x0000, // ld c, n8
                   .bc = 0x0011,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0002},
                  {.af = 0x8000, // ld a, $80
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0002},
                  {.af = 0x0000, // ld [hl-], a
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = -0x0001,
                   .sp = 0x0000,
                   .pc = 0x0001},
                  {.af = 0x0000, // ldh [c], a
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0001},
                  {.af = -0x00A0, // inc c
                   .bc = 0x0001,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0001},
                  {.af = 0x7300, // ld a, n8
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0002},
                  {.af = 0x0000, // ldh [c], a
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0001},
                  {.af = 0x0000, // ld [hl-], a
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = -0x0001,
                   .sp = 0x0000,
                   .pc = 0x0001},
                  {.af = -0x7C00, // ld a, $77
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0002},
                  {.af = 0x0000, // ld [hl], a
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0001},
                  {.af = 0x8500, // ld a, $FC
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0002},
                  {.af = 0x0000, // ldh [FF47], a
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0002},
                  {.af = 0x0000, // ld de, $0104
                   .bc = 0x0000,
                   .de = 0x0104,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0003},
                  {.af = 0x0000, // ld h, $8010
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = -0x7F14,
                   .sp = 0x0000,
                   .pc = 0x0003},
                  {.af = -0x2E00, // ld a, [de]
                   .bc = 0x0000,
                   .de = 0x0000,
                   .hl = 0x0000,
                   .sp = 0x0000,
                   .pc = 0x0001},
              }},
         {.rep = 1,
          .length = 11,
          .expected =
              {
                  .af = 0x3B40,
                  .bc = 0x039D,
                  .de = 0x0104,
                  .hl = 0x8010,
                  .sp = 0xFFFC,
                  .pc = 0x0098,
              },
          .state =
              {
                  0xFFFD,
                  0x0000,
                  0xFFFC,
                  0x002B,
                  0xFFFB,
                  0x0004,
                  0xFFFA,
                  0x00CE,
              },
          .diff = {
              {.af = 0x0000, // call $0095
               .bc = 0x0000,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = -0x0002,
               .pc = 0x006D},
              {.af = 0x0000, // ld c, a
               .bc = 0x00BC,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0000,
               .pc = 0x0001},
              {.af = 0x0000, // ld b, 8 / 2
               .bc = 0x0400,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0000,
               .pc = 0x0002},
              // .loop
              {.af = 0x0000, // push bc
               .bc = 0x0000,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = -0x0002,
               .pc = 0x0001},
              {.af = 0x0010, // rl c
               .bc = -0x0032,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0000,
               .pc = 0x0002},
              {.af = -0x3100, // rla
               .bc = 0x0000,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0000,
               .pc = 0x0001},
              {.af = 0x0000, // pop bc
               .bc = 0x0032,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0002,
               .pc = 0x0001},
              {.af = 0x0000, // rl c
               .bc = -0x0031,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0000,
               .pc = 0x0002},
              {.af = -0x6200, // rla
               .bc = 0x0000,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0000,
               .pc = 0x0001},
              {.af = 0x0030, // dec b
               .bc = -0x0100,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0000,
               .pc = 0x0001},
              {.af = 0x0000, // jr nz, .loop
               .bc = 0x0000,
               .de = 0x0000,
               .hl = 0x0000,
               .sp = 0x0000,
               .pc = -0x0009},
          }}};

RegisterDiff TESTS_DIFF[TESTS_COUNT] = {{.af = 0x0000, // boot
                                         .bc = 0x0000,
                                         .de = 0x0000,
                                         .hl = 0x0000,
                                         .sp = 0x0000,
                                         .pc = 0x0000},
                                        {.af = 0x0000, // ld sp, 0xFFFE
                                         .bc = 0x0000,
                                         .de = 0x0000,
                                         .hl = 0x0000,
                                         .sp = 0xFFFE,  // + 0xFFFE
                                         .pc = 0x0003}, // + 3
                                        {.af = 0x0080,  // xor a, a
                                         .bc = 0x0000,
                                         .de = 0x0000,
                                         .hl = 0x0000,
                                         .sp = 0x0000,
                                         .pc = 0x0001},
                                        {.af = 0x0000, // ld [hl], 0x9FFF
                                         .bc = 0x0000,
                                         .de = 0x0000,
                                         .hl = 0x9FFF,
                                         .sp = 0x0000,
                                         .pc = 0x0003},
                                        {.af = 0x0000, // ld[hl-], a
                                         .bc = 0x0000,
                                         .de = 0x0000,
                                         .hl = -0x0001,
                                         .sp = 0x0000,
                                         .pc = 0x0001},
                                        {.af = -0x0060, // bit 7, h (prefix)
                                         .bc = 0x0000,
                                         .de = 0x0000,
                                         .hl = 0x0000,
                                         .sp = 0x0000,
                                         .pc = 0x0002},
                                        {.af = 0x0000, // jr nz, n8
                                         .bc = 0x0000,
                                         .de = 0x0000,
                                         .hl = 0x0000,
                                         .sp = 0x0000,
                                         .pc = -0x0003}};

// af, bc, de, hl, sp, pc
Registers TESTS[TESTS_COUNT] = {
    {.af = 0x0000, // boot
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x0000,
     .sp = 0x0000,
     .pc = 0x0000},
    {.af = 0x0000, // ld sp, 0xFFFE
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x0000,
     .sp = 0xFFFE,
     .pc = 0x0003},
    {.af = 0x0080, // xor a, a
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x0000,
     .sp = 0xFFFE,
     .pc = 0x0004},
    {.af = 0x0080, // ld [hl], 0x9FFF
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x9FFF,
     .sp = 0xFFFE,
     .pc = 0x0007},
    {.af = 0x0080, // ld[hl-], a
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x9FFE,
     .sp = 0xFFFE,
     .pc = 0x0008},
    {.af = 0x0020, // bit 7, h (prefix)
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x9FFE,
     .sp = 0xFFFE,
     .pc = 0x000A},
    {.af = 0x0020, // jr nz, n8
     .bc = 0x0000,
     .de = 0x0000,
     .hl = 0x9FFE,
     .sp = 0xFFFE,
     .pc = 0x0007},
};
