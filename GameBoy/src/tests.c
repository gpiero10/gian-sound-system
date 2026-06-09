#include <commons.h>
#include <cpu.h>
#include <devices.h>
#include <bus.h>
#include <assert.h>


u8 romTestLD[0x8000] = 
{ 
    [0x100] = 0x21, // LD HL,C000
    [0x101] = 0x00,
    [0x102] = 0xC0,

    [0x103] = 0x3E, // LD A,12
    [0x104] = 0x12,

    [0x105] = 0x22, // LD [HL+], A

    [0x106] = 0x00,  // NOP

    [0x107] = 0x76  // HALT
};



int main()
{
    init_Devices("Vitinha de la Salada");
    cpu_init();
    
    // Ejecuto y asserto
    cpuStep(); // LD HL,C000
    assert(readCPURegister(RT_HL) == 0xC000);

    cpuStep(); // LD A, 12
    assert(readCPURegister(RT_A) == 0x12);

    cpuStep(); // LD [HL+], A
    assert(busRead(0xC000) == 0x12);

    printf("ROM_TEST LD IS OK!\n");

    return 0;
}
