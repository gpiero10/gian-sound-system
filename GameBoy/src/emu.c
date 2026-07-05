#include <emu.h>

void init_EMU(char* rom)
{
    init_Devices(rom);
    cpu_init();
}

void run_EMU()
{
    cpuRun();
}