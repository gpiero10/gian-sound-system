#pragma once

#include <commons.h>
#include <bus.h>
#include <instrucciones.h>
#include <interrupciones.h>
#include <timer.h>

typedef enum
{
    // Bit	Name	Explanation
    // 7	z	Zero flag
    // 6	n	Subtraction flag (BCD)
    // 5	h	Half Carry flag (BCD)
    // 4	c	Carry flag
    
    F_CARRY,
    F_ZERO,
    F_N_Subtract,
    F_HalfCarry,
} flag_t;

typedef struct cpuRegisters
{
    u8 a; // acumulador
 
    u8 f; // flags

    u8 b; 
    u8 c;
    u8 d;
    u8 e;
    u8 h;
    u8 l;
    u16 sp; // stack Pointer
    u16 pc; // program counter

} cpu_registers;

typedef struct cpuContext
{
    cpu_registers registers;
    
    //current fetch...
    u16 fetched_data;
    u16 mem_dest;
    bool dest_is_mem;
    u8 cur_opcode;
    instruction* currentInstruction;

    bool halted; // Esta parada la ejecucion
    bool halt_Bug; // flag que indica si ocurrio un halt bug

    bool stepping; // (?) los m-cycles que dura una ejecucion de instruccion (acumulado), me inspire en el chabon del tutubi
    bool running; // indica si el gayboy (emulador) esta encendido

    bool int_master_enabled; // IME: Interrupt master enable flag [write only]. Interno al cpu, no se puede leer (osea no se carga en ningun registro ni memoria)
    bool activando_IME; // un Flag que indica que se tiene que activar la flag int_master_enabled, simulando una especie de delay
    u8 delayPatriotico; // contador de instrucciones para delay EI
} cpu_context;

typedef void (*in_proc)(cpu_context*); // El tipo de las funciones processors
in_proc getProcessorForCurrentInst(cpu_context* ctx);

u16 readCPURegister(reg_type rt);
void writeCPURegister(reg_type rt, u16 val);

u8 getFlag(cpu_context* ctx, flag_t f);

void cpu_init();
void cpuRun();
void cpuFetch();
void cpuStep();
void cpuHalted();