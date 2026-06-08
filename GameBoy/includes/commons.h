#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define sixteenKB (1 << 14)
#define eigthKB (1 << 13)
#define fourKB (1 << 12)
#define ochoBitsBajos(x) (x & 0xFF)
#define between(x,y,e) ((e >= x) && (e <= y))

// operaciones 8-bits
#define mitadAltaByte(x) ((x & 0xF0) >> 4) 
#define mitadBajaByte(x) (x & 0x0F)

#define opCodeBits(x) ((x & 0xC0) >> 6)
#define bits01y2(x) (x & 0x7)
#define bits3y4(x) ((x & 0x18) >> 3)
#define bits4y5(x) ((x & 0x30) >> 4)
#define bits345(x) ((x & 0b00111000) >> 3)
#define bit7(x) ((x & 0x80) >> 7)
#define bit6(x) ((x & 0x40) >> 6)
#define bit5(x) ((x & 0x20) >> 5)
#define bit4(x) ((x & 0x10) >> 4)

#define isCarry(src1, src2) ((src1 + src2) > 0xFF)
#define isHalfCarry(src1, src2) (((src1 & 0xF) + (src2 & 0xF)) > 0xF)
#define isHalfBorrow(src, minuendo) ((src & 0xF) < (minuendo & 0xF))

// Flags
#define bitFlagZero(x) ((x & 0x80) >> 7)
#define bitFlagSubtraction(x) ((x & 0x40) >> 6)
#define bitFlagHalfCarry(x) ((x & 0x20) >> 5)
#define bitFlagCarry(x) ((x & 0x10) >> 4)

//op 16-bits
#define ext16bits(H,L) ((H << 8) | L)
#define byteBajoDeWord(X) ((X & 0x00FF)) 
#define byteAltoDeWord(X) ((X & 0xFF00) >> 8)

#define isCarry16(src1, src2) ((src1 + src2) > 0xFFFF)
#define isHalfCarry16(src1, src2) (((src1 & 0xFF) + (src2 & 0xF)) > 0xFF)
#define isHalfBorrow16(src, minuendo) ((src & 0xFF) < (minuendo & 0xFF))


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t i8;
typedef int16_t i16;