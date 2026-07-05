#pragma once

#include <commons.h>
#include <interrupciones.h>

typedef struct TIMER
{
    u16 systemCounter; // 0xFF04: La parte alta es div
    u8 tima; // 0xFF05
    u8 tma;  // 0xFF06
    u8 tac;  // 0xFF07

    i8 overflowTCycles;

} timer_t;

timer_t* init_Timer();
void emu_cycles(u8 mCycles);

void timerTick();

void writeTimer(u16 addr, u8 val);
u8 readTimer(u16 addr);