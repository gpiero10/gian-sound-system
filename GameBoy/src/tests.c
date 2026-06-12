#include <commons.h>
#include <cpu.h>
#include <devices.h>
#include <bus.h>
#include <assert.h>

extern cpu_context cpu_ctx;

/*
void test_LD()
{
    cpuStep(); // LD HL,C000
    assert(readCPURegister(RT_HL) == 0xC000);

    cpuStep(); // LD A, 12
    assert(readCPURegister(RT_A) == 0x12);

    cpuStep(); // LD [HL+], A
    assert(busRead(0xC000) == 0x12);
    assert(readCPURegister(RT_HL) == 0xC001);

    cpuStep(); // LD D, E      <--> D <- 0xD8
    assert(readCPURegister(RT_D) == 0xD8);

    cpuStep(); // LD [HL], D   <--> [xC001] <- 0xD8
    assert(busRead(0xC001) == 0xD8);

    cpuStep(); // LD A, [HL]   <--> A <- [xC001] = 0xD8
    assert(readCPURegister(RT_A) == 0xD8);

    cpuStep(); // LD (a16),A   <--> [xC002] <- 0xD8
    assert(busRead(0xC002) == 0xD8);

    cpuStep(); // LD A,(a16)   <--> A <- [xC000] = 0x12
    assert(readCPURegister(RT_A) == 0x12);

    cpuStep(); // LD A,(HL+)   <--> A <- [xC001] = 0xD8
    assert(readCPURegister(RT_A) == 0xD8);
    assert(readCPURegister(RT_HL) == 0xC002);

    cpuStep(); // LD (HL-),A   <--> [xC002] <- 0xD8
    assert(busRead(0xC002) == 0xD8);
    assert(readCPURegister(RT_HL) == 0xC001);

    cpuStep(); // LD A,(HL-) <--> A <- [xC001] = 0xD8
    assert(readCPURegister(RT_A) == 0xD8);
    assert(readCPURegister(RT_HL) == 0xC000);

    printf("ROM_TEST LD IS OK! ✅\n");
}
void test_INC()
{
    cpuStep(); // INC B
    assert(readCPURegister(RT_B) == 0x01);

    cpuStep(); // LD HL, 0xC000
    cpuStep(); // LD [HL], 0x00
    cpuStep(); // INC [HL]
    assert(busRead(0xC000) == 1);

    cpuStep(); // INC DE
    assert(readCPURegister(RT_DE) == 0x00D9);

    cpuStep(); //  INC SP
    assert(readCPURegister(RT_SP) == 0xFFFF);

    cpuStep(); // LD B, 0x0F
    cpuStep(); // INC B
    assert(getFlag(&cpu_ctx, F_HalfCarry) == 1);
    assert(getFlag(&cpu_ctx, F_ZERO) == 0);

    cpuStep(); // LD [HL], 0x0F
    cpuStep(); // INC [HL]
    assert(getFlag(&cpu_ctx, F_HalfCarry) == 1);
    assert(getFlag(&cpu_ctx, F_ZERO) == 0);
    
    printf("ROM_TEST INC IS OK! ✅\n");
}
u8 romTestLD[0x8000] = 
{ 
    // LD r,r       ✅
    // LD r,d8      ✅
    // LD rr,d16    ✅
    // LD (HL),D    ✅  
    // LD A,(HL)    ✅ 
    // LD (a16),A   ✅
    // LD A,(a16)   ✅
    // LD (HL+),A   ✅
    // LD A,(HL+)   ✅
    // LD (HL-),A   ✅
    // LD A,(HL-)   ✅

    [0x100] = 0x21, // LD HL, xC000
    [0x101] = 0x00,
    [0x102] = 0xC0,

    [0x103] = 0x3E, // LD A, x12
    [0x104] = 0x12,

    [0x105] = 0x22, // LD [HL+], A  <--> [xC000] <- 12

    [0x106] = 0x53, // LD D, E      <--> D <- 0xD8

    [0x107] = 0x72, // LD [HL], D   <--> [xC001] <- 0xD8

    [0x108] = 0x7E, // LD A, [HL]   <--> A <- [xC001] = 0xD8

    [0x109] = 0xEA, // LD (a16),A   <--> [xC002] <- 0xD8
    [0x10A] = 0x02,
    [0x10B] = 0xC0,

    [0x10C] = 0xFA, // LD A,(a16)   <--> A <- [xC000] = 12
    [0x10D] = 0x00,
    [0x10E] = 0xC0,
    
    [0x10F] = 0x2A, // LD A,(HL+)   <--> A <- [xC001] = 0xD8
    
    [0x110] = 0x32, // LD (HL-),A   <--> [xC002] <- 0xD8

    [0x111] = 0x3A, // LD A,(HL-) <--> A <- [xC001] = 0xD8

};
u8 romTestINC[0x1000] =
{
    // INC: INC r8, INC [HL], INC r16, INC SP
    
    [0x100] = 0x04, // INC B

    // INC [HL]
    [0x101] = 0x21, // LD HL, n16
    [0x102] = 0x00, 
    [0x103] = 0xC0, 

    [0x104] = 0x36, // LD [HL], n8
    [0x105] = 0x00,
    
    [0x106] = 0x34, // INC [HL]

    [0x107] = 0x13, // INC DE
    [0x108] = 0x33, // INC SP
    
    //HalfCarry Test r8
    [0x109] = 0x06, // LD B, n8
    [0x10A] = 0x0F, // n8

    [0x10B] = 0x04, // INC B

    //HalfCarry Test [a16]
    [0x10C] = 0x36, // LD [HL], n8
    [0x10D] = 0x0F,

    [0x10E] = 0x34, // INC [HL]
    
};
*/

int main()
{
    init_Devices("rom/tests/06-ld r,r.gb");
    cpu_init();
    cpuRun();
    printf("TESTS PASSED UNTIL HALT");
    return 0;
}

/*

PC=0206 OP=2A   LD A, [HL+]
FLAGS: N = 0, Z = 0, C = 0, HC = 0
AF=EC00 BC=C30D DE=C3FF HL=4400 SP=DFFF

PC=0207 OP=12   LD [DE], A
FLAGS: N = 0, Z = 0, C = 0, HC = 0
AF=EC00 BC=C30D DE=C3FF HL=4400 SP=DFFF

PC=0208 OP=1C   INC E
FLAGS: N = 0, Z = 1, C = 0, HC = 1
AF=ECA0 BC=C30D DE=C300 HL=4400 SP=DFFF

PC=0209 OP=20   JR NZ, e8
FLAGS: N = 0, Z = 1, C = 0, HC = 1
AF=ECA0 BC=C30D DE=C300 HL=4400 SP=DFFF

PC=020B OP=14   INC D
FLAGS: N = 0, Z = 0, C = 0, HC = 0
AF=EC00 BC=C30D DE=C400 HL=4400 SP=DFFF

PC=020C OP=0D   DEC C
FLAGS: N = 1, Z = 0, C = 0, HC = 0
AF=EC40 BC=C30C DE=C400 HL=4400 SP=DFFF

PC=020D OP=20   JR NZ, e8
FLAGS: N = 1, Z = 0, C = 0, HC = 0
AF=EC40 BC=C30C DE=C400 HL=4400 SP=DFFF


*/
