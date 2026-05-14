#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define sixteenKB (1 << 14)
#define eigthKB (1 << 13)
#define fourKB (1 << 12)
#define ochoBitsBajos(x) (x & 0x11)
#define between(x,y,e) ((e >= x) && (e <= y))

// operaciones 8-bits
#define mitadAltaByte(x) ((x & 0xF0) >> 4) 
#define mitadBajaByte(x) (x & 0x0F)

#define opCodeBits(x) ((x & 0xC0) >> 6)
#define bits01y2(x) (x & 0x7)
#define bits3y4(x) ((x & 0x18) >> 3)
#define bits4y5(x) ((x & 0x30) >> 4)
#define bits345(x) ((x & 0b00111000) >> 3)

//op 16-bits
#define ext16bits(H,L) ((H << 8) | L);
#define byteBajoDeWord(X) ((X & 0x00FF)) 
#define byteAltoDeWord(X) ((X & 0xFF00) >> 8)

typedef uint8_t u8;
typedef uint16_t u16;