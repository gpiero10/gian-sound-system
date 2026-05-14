#include "commons.h"
#include "bus.h"

//          0	1	2	3	4	5	 6	    7
// r8	    b	c	d	e	h	l	[hl]    a
// r16	    bc	de	hl	sp	
// r16stk	bc	de	hl	af	
// r16mem	bc	de	hl+	hl-	
// cond	    nz	z	nc	c	
// b3	    A 3-bit bit index
// tgt3	    rst's target address, divided by 8
// imm8	    The following byte
// imm16	The following two bytes, in little-endian order

#define HLplus 2
#define HLminus 3

#define Br8id 0b000
#define Cr8id 0b001
#define Dr8id 0b010
#define Er8id 0b011
#define Hr8id 0b100
#define Lr8id 0b101
//#define registerHLid 0b110
#define Ar8id 0b111

// ID binario	ID decimal	Par de registros (según contexto)
// 00	        0	        BC
// 01	        1	        DE
// 10	        2	        HL
// 11	        3	        SP (para LD rp, imm16, ADD HL, rp, INC rp, DEC rp), AF (solo para PUSH y POP)
#define BCr16id 0b00
#define DEr16id 0b01
#define HLr16id 0b10
#define AFr16id 0b11
#define SPr16id 0x67 // six seven six seven six seven six seven six seven
#define PCr16id 0x69 // JA JA JA

typedef struct cpuRegisters
{
    uint8_t a; // acumulador
 
    // Bit	Name	Explanation
    // 7	z	Zero flag
    // 6	n	Subtraction flag (BCD)
    // 5	h	Half Carry flag (BCD)
    // 4	c	Carry flag
    uint8_t f; // flags

    uint8_t b; 
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t h;
    uint8_t l;
    uint16_t sp; // stack Pointer
    uint16_t pc; // program counter

} cpu_registers;

typedef struct cpuContext
{
    cpu_registers* registers;
    
    // fetched (instruccion ejecutada en este momento), idk bout this ... Sure!
    instruction_t* currentInstruction;

    bool halt; // Esta parada la ejecucion
    bool stepping; // (?) los m-cycles que dura una ejecucion de instruccion (acumulado), me inspire en el chabon del tutubi

} cpu_context;

typedef struct instruction
{
    /* data */
    u8 opCode;
    u8 code;
    u16 romAddress;
    
} instruction_t;

void cpuCycle();
void ld_R8_imm8();
void ld_imm16_sp();
void ld_A_R16mem();
void ld_R16mem_A();
void ld_R16_Imm16(u8 dest, u16 imm16);
void jp_hl();
void jp_imm16();
void jp_Cond_Imm16();
uint8_t readCPURegister8ByIndex(uint8_t source);
void writeCPURegister8ByIndex(uint8_t dest, uint8_t val);