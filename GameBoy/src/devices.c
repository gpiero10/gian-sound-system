#include "devices.h"

device_t* devices[cantDevices];
u8 dmaTransferActive;

// Inicializar all devices para un ROM dado
void init_Devices(char* gameROMFilePath)
{
    dmaTransferActive = false;
    memset(&devices, 0, sizeof(device_t) * cantDevices);
    
    // cartridge_t* cartucho;
    cartridge_t* cartucho = initCartridge(gameROMFilePath);
    device_t cartridge;
    cartridge.ctx = cartucho;
    cartridge.read = readCartridge;
    cartridge.write = writeCartridge;
    cartridge.enRango = enRangoCartucho;
    devices[cartridgeId] = &cartridge;

    // VRAM
    device_t vram;
    vram_t vRam;
    vram.ctx = &vRam;
    vram.read = readVRAM;
    vram.write = writeVRAM;
    vram.enRango = enRangoVRAM;
    devices[vramId] = &vram;

    // Work Ram
    device_t workRAM;
    workRam_t workRam;
    workRAM.ctx = &workRam;
    workRAM.read = readWorkRam;
    workRAM.write = writeWorkRam;
    workRAM.enRango = enRangoWRAM;
    devices[workRAMId] = &workRAM;

    // OAM
    device_t oam;
    OAM_t oamTableOfSprites;  
    oam.ctx = &oamTableOfSprites;
    oam.read = readOAM;
    oam.write = writeOAM;
    oam.enRango = enRangoWRAM;
    devices[oamId] = &oam;

    // IO Registers
    device_t ioRegisters;
    IORegisters_t IORegisters;
    ioRegisters.ctx = &IORegisters;
    ioRegisters.read = readRegister;
    ioRegisters.write = writeRegister;
    ioRegisters.enRango = enRangoIORegisters;
    devices[ioRegsId] = &ioRegisters;

    // HRAM
    device_t hRAM;
    hram_t HRam;
    hRAM.ctx = &HRam;
    hRAM.read = readHRam;
    hRAM.write = writeHRam;
    hRAM.enRango = enRangoHRAM;
    devices[hramId] = &hRAM;

    // IE Register
    device_t ieRegs;
    IEregister_t interruptControl;
    ieRegs.ctx = &interruptControl;
    ieRegs.read = readInterruptRegisterControl;
    ieRegs.write = writeInterruptRegisterControl;
    ieRegs.enRango = enRangoIEReg;
    devices[ieRegsId] = &ieRegs;    
}

// getter de puntero a device
device_t* getDeviceByIndex(u8 i)
{
    return devices[i];
}

// Auxiliares Read/Write registros i/o
u8 buscarRegistroYLeerByte(u16 addr)
{
    // un Quilombo
    uint8_t dato = -1;
    IORegisters_t* ctx = devices[ioRegsId]->ctx; 

    if (0xFF00 == addr)
    {
        dato = ctx->joypad;
    }
    else if (between(0xFF01, 0xFF02, addr))
    {
        dato = ctx->serialTransfer[addr - 0xFF01];
    }
    else if (between(0xFF04, 0xFF07, addr))
    {
        dato = ctx->timerAndDivider[addr - 0xFF04];
    }
    else if (0xFF0F == addr)
    {
        dato = ctx->interrupts;
    }
    else if (between(0xFF10, 0xFF26, addr))
    {
        dato = ctx->audio[addr - 0xFF10];
    }
    else if (between(0xFF30, 0xFF3F, addr))
    {
        dato = ctx->wavePattern[addr - 0xFF30];
    }
    else if (between(0xFF40, 0xFF4B, addr))
    {
        dato = ctx->LCDControlSPSP[addr - 0xFF40];
    }
    else if (0xFF46 == addr)
    {
        dato = ctx->oamDmaTransfer;
    }
    else if (between(0xFF4C, 0xFF4D, addr))
    {
        dato = ctx->cgbKey0AndKey1[addr-0xFF4C];
    }
    else if (0xFF4F == addr)
    {
        dato = ctx->cgbVramBankSelect;
    }
    else if (0xFF50 == addr)
    {
        dato = ctx->bootRomMappingControl;
    }
    else if (between(0xFF51, 0xFF55, addr))
    {
        dato = ctx->cgbVramDma[addr-0xFF51];
    }
    else if (0xFF56 == addr)
    {
        dato = ctx->cgbIRPort;
    }
    else if (between(0xFF68, 0xFF6B, addr))
    {
        dato = ctx->cgbBgObjPalletes[addr-0xFF68];
    }
    else if (0xFF6C == addr)
    {
        dato = ctx->cgbObjectPriorityMode;
    }
    else if (0xFF70 == addr)
    {
        dato = ctx->cgbWramBankSelect;
    }

    return dato;
}
void buscarRegistroYEscribirByte(u16 addr, u8 val)
{
    // un Quilombo
    IORegisters_t* ctx = devices[ioRegsId]->ctx; 
    
    if (0xFF00 == addr)
    {
        ctx->joypad = val;
    }
    else if (between(0xFF01, 0xFF02, addr))
    {
        ctx->serialTransfer[addr - 0xFF01] = val;
    }
    else if (between(0xFF04, 0xFF07, addr))
    {
        ctx->timerAndDivider[addr - 0xFF04] = val;
    }
    else if (0xFF0F == addr)
    {
        ctx->interrupts = val;
    }
    else if (between(0xFF10, 0xFF26, addr))
    {
        ctx->audio[addr - 0xFF10] = val;
    }
    else if (between(0xFF30, 0xFF3F, addr))
    {
        ctx->wavePattern[addr - 0xFF30] = val;
    }
    else if (between(0xFF40, 0xFF4B, addr))
    {
        ctx->LCDControlSPSP[addr - 0xFF40] = val;
    }
    else if (0xFF46 == addr)
    {
        ctx->oamDmaTransfer = val;
    }
    else if (between(0xFF4C, 0xFF4D, addr))
    {
        ctx->cgbKey0AndKey1[addr-0xFF4C] = val;
    }
    else if (0xFF4F == addr)
    {
        ctx->cgbVramBankSelect = val;
    }
    else if (0xFF50 == addr)
    {
        ctx->bootRomMappingControl = val;
    }
    else if (between(0xFF51, 0xFF55, addr))
    {
        ctx->cgbVramDma[addr-0xFF51] = val;
    }
    else if (0xFF56 == addr)
    {
        ctx->cgbIRPort = val;
    }
    else if (between(0xFF68, 0xFF6B, addr))
    {
        ctx->cgbBgObjPalletes[addr-0xFF68] = val;
    }
    else if (0xFF6C == addr)
    {
        ctx->cgbObjectPriorityMode = val;
    }
    else if (0xFF70 == addr)
    {
        ctx->cgbWramBankSelect = val;
    }
}

// DMA TRANSFER
void dmaOamTransfer(u8 hAddr)
{
    // Inhabilito el uso del read/write (bus)
    dmaTransferActive = true;
    u16 addr = hAddr << 8; // $FF46 dma state register

    OAM_t* oam = devices[oamId]->ctx;

    if (between(0x0, 0x7FFF, addr) || between(0xA000, 0xBFFF, addr)) // Direccion de ROM o dir extRam, if any (8kib)
    {
        for (int i = 0; i < 160; i++)
        {
            ((OAM_t*)(devices[oamId]->ctx))->spritesTable[i] = readCartridge(addr);
            addr ++;
        }
    }
    else if (between(0xC000, 0xDFFF, addr)) // dir work ram (8kib)
    {
        // Como no implemento cbg (a color) no hay bancos switcheables 
        for (int i = 0; i < 160; i++)
        {
            ((OAM_t*)(devices[oamId]->ctx))->spritesTable[i] = readWorkRam(addr);
            addr ++;
        }
    }
    dmaTransferActive = 0;
}
u8 getDMATransferState() { return dmaTransferActive;}

// Read Devices
u8 readVRAM(u16 addr)
{
    
    u8 state = buscarRegistroYLeerByte(0xFF41); //$FF41, ppu mode
    if (state == 3) //Mode 3: PPU Drawing pixels
    {
        return -1;
    }
    else
    {
        return ((vram_t*)(devices[vramId]->ctx))->data[addr - 0x8000];
    }            
}
u8 readWorkRam(u16 addr)
{
    // Como no implemento cbg (a color) no hay bancos switcheables 
    return ((workRam_t*)(devices[workRAMId]->ctx))->data[addr - 0xC000];
}
u8 readOAM(u16 addr)
{
    u8 state = buscarRegistroYLeerByte(0xFF41); //$FF41, ppu mode
    if ((state == 2) || (state == 3)) // Mode 2: Searching for sprites in OAM || Mode 3: PPU Drawing pixels 
    {
        return -1;
    }
    else
    {
        return ((OAM_t*)(devices[oamId]->ctx))->spritesTable[addr-0xFE00];
    }   
}
u8 readRegister(u16 addr)
{
    return buscarRegistroYLeerByte(addr);
}
u8 readHRam(u16 addr)
{
    return ((hram_t*)(devices[hramId]->ctx))->data[addr-0xFF80];
}
u8 readInterruptRegisterControl()
{
    return ((IEregister_t*)(devices[ieRegsId]->ctx))->registerControl;
}

// Write Devices
void writeVRAM(u16 addr, u8 val)
{
    u8 state = buscarRegistroYLeerByte(0xFF41); //$FF41, ppu mode
    if (state == 3) //Mode 3: PPU Drawing pixels
    {
        return;
    }
    else
    {
        ((vram_t*)(devices[vramId]->ctx))->data[addr-0x8000] = val;
    } 
}
void writeWorkRam(u16 addr, u8 val)
{
    // Como no implemento cbg (a color) no hay bancos switcheables 
    ((workRam_t*)(devices[workRAMId]->ctx))->data[addr-0xC000] = val;
}
void writeOAM(u16 addr, u8 val)
{
    u8 state = buscarRegistroYLeerByte(0xFF41); //$FF41, ppu mode
    if ((state == 2) || (state == 3)) // Mode 2: Searching for sprites in OAM || Mode 3: PPU Drawing pixels 
    {
        return;
    }
    else
    {
        ((OAM_t*)(devices[oamId]->ctx))->spritesTable[addr-0xFE00] = val;    
    }  
}
void writeRegister(u16 addr, u8 val)
{
    buscarRegistroYEscribirByte(addr, val);
}
void writeHRam(u16 addr, u8 val)
{
    // Unica area de memoria disponible durante una transferencia DMA to OAM (ram/rom -> OAM)
    ((hram_t*)(devices[hramId]->ctx))->data[addr-0xFF80] = val;
}
void writeInterruptRegisterControl(u8 val)
{
    ((IEregister_t*)(devices[ieRegsId]->ctx))->registerControl = val;
}


// En rango Devices
bool enRangoCartucho(u16 addr) { return cartridgeRange(addr) || extRAMRange(addr);}

bool enRangoVRAM(u16 addr) { return vramRange(addr);}

bool enRangoWRAM(u16 addr) { return workRAMRange(addr);}

bool enRangoOAM(u16 addr) { return oamRange(addr);}

bool enRangoIORegisters(u16 addr) { return ioRegistersRange(addr);}

bool enRangoHRAM(u16 addr) { return hramRange(addr);}

bool enRangoIEReg(u16 addr) { return addr == ieRegisterAddr;}