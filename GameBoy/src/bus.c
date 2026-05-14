#include "bus.h"
// Abstraccion importante porque el cpu le escribe al bus, que es el que finalmente se encarga 
// de "comunicarse" con el dispositivo correspondiente a la direccion accedida

// typedef struct Memory
// {
//     cartridge_t* cartucho;
//     vram_t vram;
//     workRam_t workRam;
//     OAM_t oamTableOfSprites;  
//     IORegisters_t IORegisters;
//     hram_t HRam;
//     IEregister_t interruptControl;

//     uint8_t dmaTransferState; // 1 si hay una transferencia DMA-OAM activa (entonces ademas solo se puede usar la Hram), 0 si no.

// } memory_t;

u8 busRead(uint16_t addr)
{   
    if (dmaTransferState() == 1) 
    {
        // Si hay transferencia DMA -> OAM, entonces el bus esta inhabilitado para el cpu
        // exceptuando la HRAM
        if (between(0xFF80, 0xFFFE, addr)) // HRAM (127 bytes)
        {
            return readHRam(addr);
        } else {
            return -1;
        }
    }
    else if (between(0x0, 0x7FFF, addr)) // Direccion de ROM
    {
        // No se puede escribir en ROM pero, se configura el MBC en caso de haberlo
        return readCartridge(addr);
    }
    else if(between(0x8000, 0x9FFF, addr)) //direccion Vram (8kib)
    {
        //memory.vram.data[addr-0x8000] = val;
        return readVRAM(addr);
    }
    else if(between(0xA000, 0xBFFF, addr)) //dir extRam, if any (8kib)
    {
        return readCartridge(addr);
    }
    else if (between(0xC000, 0xDFFF, addr)) // dir work ram (8kib)
    {
        // Como no implemento cbg (a color) no hay bancos switcheables 
        return readWorkRam(addr);
    }
    else if (between(0xFE00, 0xFE9F, addr)) // OAM (tabla de atributos de sprites, 40 entradas de 4 bytes)
    {
        return readOAM(addr);
    }
    else if (between(0xFF00, 0xFF7F, addr)) //IO Registers (127 bytes)
    {
        return readRegister(addr);
    }
    else if (between(0xFF80, 0xFFFE, addr))
    {
        // Unica area de memoria disponible durante una transferencia DMA to OAM (ram/rom -> OAM)
        return readHRam(addr);
    }
    else if (between(0xFFFF, 0xFFFF, addr))
    {
        return readInterruptRegisterControl();
    }

    return 0xFF;
}

void busWrite(uint16_t addr, uint8_t val)
{   
    if (dmaTransferState() == 1) 
    {
        // Si hay transferencia DMA -> OAM, entonces el bus esta inhabilitado para el cpu
        // exceptuando la HRAM
        if (0xFF80 <= addr	&& addr <= 0xFFFE) // HRAM (127 bytes)
        {
            writeHRam(addr, val);
        } else {
            return;
        }
    }
    else if (between(0x0, 0x7FFF, addr)) // Direccion de ROM
    {
        // No se puede escribir en ROM pero, se configura el MBC en caso de haberlo
        writeCartridge(addr, val);
    }
    else if(between(0x8000, 0x9FFF, addr)) //direccion Vram (8kib)
    {
        //memory.vram.data[addr-0x8000] = val;
        writeVRAM(addr, val);
    }
    else if(between(0xA000, 0xBFFF, addr)) //dir extRam, if any (8kib)
    {
        writeCartridge(addr, val);
    }
    else if (between(0xC000, 0xDFFF, addr)) // dir work ram (8kib)
    {
        // Como no implemento cbg (a color) no hay bancos switcheables 
        writeWorkRam(addr, val);
    }
    else if (between(0xFE00, 0xFE9F, addr)) // OAM (tabla de atributos de sprites, 40 entradas de 4 bytes)
    {
        writeOAM(addr, val);
    }
    else if (between(0xFF00, 0xFF7F, addr)) //IO Registers (127 bytes)
    {
        writeRegister(addr, val);
    }
    else if (between(0xFF80, 0xFFFE, addr))
    {
        // Unica area de memoria disponible durante una transferencia DMA to OAM (ram/rom -> OAM)
        writeHRam(addr, val);
    }
    else if (between(0xFFFF, 0xFFFF, addr))
    {
        writeInterruptRegisterControl(val);
    }
    return;
}