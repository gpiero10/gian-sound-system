#pragma once

#include <commons.h>

// INT $40 — VBlank interrupt
// INT $48 — STAT interrupt
// INT $50 — Timer interrupt
// INT $58 — Serial interrupt
// INT $60 — Joypad interrupt

typedef struct cpuContext cpu_context;

typedef enum {
    //      7	6	5	  4	     3	     2	    1	  0
    //IE/IF 	        Joypad	Serial	Timer	LCD	 VBlank
    
    bit_vblank,
    bit_stat,
    bit_timer,
    bit_serial,
    bit_joypad
} interrupt_bit;

void interruptCheck(cpu_context *ctx);


