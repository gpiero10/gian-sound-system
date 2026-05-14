#include "../includes/memory.h"
// Mapa de Memoria
// 0x0000–0x7FFF → ROM
// 0x8000–0x9FFF → VRAM
// 0xA000–0xBFFF → External RAM
// 0xC000–0xDFFF → Work RAM
// 0xFF00–0xFF7F → I/O Registers

memory_t memory;

uint8_t dmaTransferState() { return memory.dmaTransferState;}

uint8_t buscarRegistroYLeerByte(uint16_t addr)
{
    // un Quilombo
    uint8_t dato = -1;

    if (0xFF00 == addr)
    {
        dato = memory.IORegisters.joypad;
    }
    else if (between(0xFF01, 0xFF02, addr))
    {
        dato = memory.IORegisters.serialTransfer[addr - 0xFF01];
    }
    else if (between(0xFF04, 0xFF07, addr))
    {
        dato = memory.IORegisters.timerAndDivider[addr - 0xFF04];
    }
    else if (0xFF0F == addr)
    {
        dato = memory.IORegisters.interrupts;
    }
    else if (between(0xFF10, 0xFF26, addr))
    {
        dato = memory.IORegisters.audio[addr - 0xFF10];
    }
    else if (between(0xFF30, 0xFF3F, addr))
    {
        dato = memory.IORegisters.wavePattern[addr - 0xFF30];
    }
    else if (between(0xFF40, 0xFF4B, addr))
    {
        dato = memory.IORegisters.LCDControlSPSP[addr - 0xFF40];
    }
    else if (0xFF46 == addr)
    {
        dato = memory.IORegisters.oamDmaTransfer;
    }
    else if (between(0xFF4C, 0xFF4D, addr))
    {
        dato = memory.IORegisters.cgbKey0AndKey1[addr-0xFF4C];
    }
    else if (0xFF4F == addr)
    {
        dato = memory.IORegisters.cgbVramBankSelect;
    }
    else if (0xFF50 == addr)
    {
        dato = memory.IORegisters.bootRomMappingControl;
    }
    else if (between(0xFF51, 0xFF55, addr))
    {
        dato = memory.IORegisters.cgbVramDma[addr-0xFF51];
    }
    else if (0xFF56 == addr)
    {
        dato = memory.IORegisters.cgbIRPort;
    }
    else if (between(0xFF68, 0xFF6B, addr))
    {
        dato = memory.IORegisters.cgbBgObjPalletes[addr-0xFF68];
    }
    else if (0xFF6C == addr)
    {
        dato = memory.IORegisters.cgbObjectPriorityMode;
    }
    else if (0xFF70 == addr)
    {
        dato = memory.IORegisters.cgbWramBankSelect;
    }

    return dato;
}

void buscarRegistroYEscribirByte(uint16_t addr, uint8_t val)
{
     // un Quilombo
    if (0xFF00 == addr)
    {
        memory.IORegisters.joypad = val;
    }
    else if (between(0xFF01, 0xFF02, addr))
    {
        memory.IORegisters.serialTransfer[addr - 0xFF01] = val;
    }
    else if (between(0xFF04, 0xFF07, addr))
    {
        memory.IORegisters.timerAndDivider[addr - 0xFF04] = val;
    }
    else if (0xFF0F == addr)
    {
        memory.IORegisters.interrupts = val;
    }
    else if (between(0xFF10, 0xFF26, addr))
    {
        memory.IORegisters.audio[addr - 0xFF10] = val;
    }
    else if (between(0xFF30, 0xFF3F, addr))
    {
        memory.IORegisters.wavePattern[addr - 0xFF30] = val;
    }
    else if (between(0xFF40, 0xFF4B, addr))
    {
        memory.IORegisters.LCDControlSPSP[addr - 0xFF40] = val;
    }
    else if (0xFF46 == addr)
    {
        memory.IORegisters.oamDmaTransfer = val;
    }
    else if (between(0xFF4C, 0xFF4D, addr))
    {
        memory.IORegisters.cgbKey0AndKey1[addr-0xFF4C] = val;
    }
    else if (0xFF4F == addr)
    {
        memory.IORegisters.cgbVramBankSelect = val;
    }
    else if (0xFF50 == addr)
    {
        memory.IORegisters.bootRomMappingControl = val;
    }
    else if (between(0xFF51, 0xFF55, addr))
    {
        memory.IORegisters.cgbVramDma[addr-0xFF51] = val;
    }
    else if (0xFF56 == addr)
    {
        memory.IORegisters.cgbIRPort = val;
    }
    else if (between(0xFF68, 0xFF6B, addr))
    {
        memory.IORegisters.cgbBgObjPalletes[addr-0xFF68] = val;
    }
    else if (0xFF6C == addr)
    {
        memory.IORegisters.cgbObjectPriorityMode = val;
    }
    else if (0xFF70 == addr)
    {
        memory.IORegisters.cgbWramBankSelect = val;
    }
}

void dmaOamTransferBegins()
{
    // Inhabilito el uso del read/write (bus)
    memory.dmaTransferState = 1;
    uint16_t addr = memory.IORegisters.oamDmaTransfer << 8;
    
    if (between(0x0, 0x7FFF, addr) || between(0xA000, 0xBFFF, addr)) // Direccion de ROM o dir extRam, if any (8kib)
    {
        for (int i = 0; i < 160; i++)
        {
            memory.oamTableOfSprites.spritesTable[i] = readCartridge(addr);
            addr ++;
        }
    }
    else if (between(0xC000, 0xDFFF, addr)) // dir work ram (8kib)
    {
        // Como no implemento cbg (a color) no hay bancos switcheables 
        for (int i = 0; i < 160; i++)
        {
            memory.oamTableOfSprites.spritesTable[i] = memory.workRam.data[addr - 0xC000];
            addr ++;
        }
    }
    
    memory.dmaTransferState = 0;
}

void initMemory(char* gameROMFilePath)
{   
    // Inicializo todas las memorias de memory, ie cada uno de los "segmentos"
    // typedef struct Memory
    // {
    //     cartridge_t* cartucho; ready
    //     vram_t* vram; ready
    //     workRam_t* workRam; ready
    //     OAM_t* oamTableOfSprites; ready  
    //     IORegisters_t* IORegisters; 
    //     hram_t* HRam; ready
    //     IEregister_t* interruptControl;

    //     uint8_t dmaTransferState; // 1 si hay una transferencia DMA-OAM activa (entonces ademas solo se puede usar la Hram), 0 si no.

    // } memory_t;

    // seteo ceros en el espacio de memory
    memset(&memory, 0, sizeof(memory_t));
    
    cartridge_t* cartucho = initCartridge(gameROMFilePath);
    memory.cartucho = cartucho;
    memory.dmaTransferState = 0;
    memory.workRam.cantidadDeBancos = 2;

    return;
}

uint8_t readVRAM(uint16_t addr)
{
    uint8_t state = memory.IORegisters.LCDControlSPSP[1]; //$FF41, ppu mode
    if (state == 3) //Mode 3: PPU Drawing pixels
    {
        return -1;
    }
    else
    {
        return memory.vram.data[addr - 0x8000];
    }            
}

uint8_t readWorkRam(uint16_t addr)
{
    // Como no implemento cbg (a color) no hay bancos switcheables 
    return memory.workRam.data[addr - 0xC000];
}

uint8_t readOAM(uint16_t addr)
{
    uint8_t state = memory.IORegisters.LCDControlSPSP[1]; //$FF41, ppu mode
    if ((state == 2) || (state == 3)) // Mode 2: Searching for sprites in OAM || Mode 3: PPU Drawing pixels 
    {
        return -1;
    }
    else
    {
        return memory.oamTableOfSprites.spritesTable[addr-0xFE00];
    }   
}

uint8_t readRegister(uint16_t addr)
{
    return buscarRegistroYLeerByte(addr);
}

uint8_t readHRam(uint16_t addr)
{
    return memory.HRam.data[addr-0xFF80];
}

uint8_t readInterruptRegisterControl()
{
    return memory.interruptControl.registerControl;
}

void writeVRAM(uint16_t addr, uint8_t val)
{
    uint8_t state = memory.IORegisters.LCDControlSPSP[1]; //$FF41, ppu mode
    if (state == 3) //Mode 3: PPU Drawing pixels
    {
        return;
    }
    else
    {
        memory.vram.data[addr-0x8000] = val;
    } 
}

void writeWorkRam(uint16_t addr, uint8_t val)
{
    // Como no implemento cbg (a color) no hay bancos switcheables 
    memory.workRam.data[addr-0xC000] = val;
}

void writeOAM(uint16_t addr, uint8_t val)
{
     uint8_t state = memory.IORegisters.LCDControlSPSP[1]; //$FF41, ppu mode
    if ((state == 2) || (state == 3)) // Mode 2: Searching for sprites in OAM || Mode 3: PPU Drawing pixels 
    {
        return;
    }
    else
    {
        memory.oamTableOfSprites.spritesTable[addr-0xFE00] = val;    
    }  
}

void writeRegister(uint16_t addr, uint8_t val)
{
    buscarRegistroYEscribirByte(addr, val);
}

void writeHRam(uint16_t addr, uint8_t val)
{
    // Unica area de memoria disponible durante una transferencia DMA to OAM (ram/rom -> OAM)
    memory.HRam.data[addr-0xFF80] = val;
}

void writeInterruptRegisterControl(uint8_t val)
{
    memory.interruptControl.registerControl = val;
}