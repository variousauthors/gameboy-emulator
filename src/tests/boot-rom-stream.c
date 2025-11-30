
#include "boot-rom-stream.h"

RegisterDiffGroup TEST_GROUPS[TEST_GROUP_COUNT] = {
    {
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
                 {.af = 0x0000, // ld [hl], 0x9FFF
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
     .length = 5,
     .expected =
         {
             .af = 0x80A0,
             .bc = 0x0011,
             .de = 0x0000,
             .hl = 0xFF25,
             .sp = 0xFFFE,
             .pc = 0x0015,
         },
     .diff = {
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
