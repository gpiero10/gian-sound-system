#include "commons.h"
#include "cartridge.h"

#define ochoBitsBajos(x) (x & 0x11)

// 0000	3FFF	16 KiB ROM bank 00	From cartridge, usually a fixed bank
// 4000	7FFF	16 KiB ROM Bank 01–NN	From cartridge, switchable bank via mapper (if any)
// 8000	9FFF	8 KiB Video RAM (VRAM)	In CGB mode, switchable bank 0/1
// A000	BFFF	8 KiB External RAM	From cartridge, switchable bank if any
// C000	CFFF	4 KiB Work RAM (WRAM)	
// D000	DFFF	4 KiB Work RAM (WRAM)	In CGB mode, switchable bank 1–7
// E000	FDFF	Echo RAM (mirror of C000–DDFF)	Nintendo says use of this area is prohibited.
// FE00	FE9F	Object attribute memory (OAM)	
// FEA0	FEFF	Not Usable	Nintendo says use of this area is prohibited.
// FF00	FF7F	I/O Registers	
// FF80	FFFE	High RAM (HRAM)	
// FFFF	FFFF	Interrupt Enable register (IE)

typedef struct OAM
{
    // Tabla de 40 entradas de 4 bytes, donde cada entrada es un sprite
    uint8_t spritesTable[160];
} OAM_t;

typedef struct
{
    uint8_t data[eigthKB]; //(8 kib)
} vram_t;

typedef struct
{
    uint8_t data[eigthKB];
    uint8_t cantidadDeBancos; //Hasta 8 en cgb mode (0 y 1-7 switcheables)
    
} workRam_t;

typedef struct
{
    // $FF00		    DMG    Joypad input
    uint8_t joypad; 
    
    // $FF01	$FF02	DMG	Serial transfer
    uint8_t serialTransfer[2]; 

    // $FF04	$FF07	DMG	Timer and divider
    uint8_t timerAndDivider[4];

    // $FF0F		    DMG	Interrupts
    uint8_t interrupts;

    // $FF10	$FF26	DMG	Audio
    uint8_t audio[23];

    // $FF30	$FF3F	DMG	Wave pattern
    uint8_t wavePattern[16];

    // $FF40	$FF4B	DMG	LCD Control, Status, Position, Scrolling, and Palettes
    uint8_t LCDControlSPSP[12];

    // $FF46		    DMG	OAM DMA transfer
    uint8_t oamDmaTransfer;

    // $FF4C	$FF4D	CGB	KEY0 and KEY1
    uint8_t cgbKey0AndKey1[2];

    // $FF4F		    CGB	VRAM Bank Select
    uint8_t cgbVramBankSelect;

    // $FF50		    DMG	Boot ROM mapping control
    uint8_t bootRomMappingControl;

    // $FF51	$FF55	CGB	VRAM DMA
    uint8_t cgbVramDma[5];

    // $FF56		    CGB	IR port
    uint8_t cgbIRPort;

    // $FF68	$FF6B	CGB	BG / OBJ Palettes
    uint8_t cgbBgObjPalletes[4];

    // $FF6C		    CGB	Object priority mode
    uint8_t cgbObjectPriorityMode;

    // $FF70		    CGB	WRAM Bank Select
    uint8_t cgbWramBankSelect;
} IORegisters_t;

typedef struct HRAM
{
    /* Unica RAM accesible durante un DMA transfer */
    uint8_t data[127];
} hram_t;

typedef struct InterruptEnableRegister
{
    uint8_t registerControl;
} IEregister_t;

typedef struct Memory
{
    cartridge_t* cartucho;
    vram_t vram;
    workRam_t workRam;
    OAM_t oamTableOfSprites;  
    IORegisters_t IORegisters;
    hram_t HRam;
    IEregister_t interruptControl;

    uint8_t dmaTransferState; // 1 si hay una transferencia DMA-OAM activa (entonces ademas solo se puede usar la Hram), 0 si no.

} memory_t;

//uint8_t read(uint16_t addr);
uint8_t dmaTransferState();
uint8_t buscarRegistroYLeerByte(uint16_t addr);
void write(uint16_t addr, uint8_t val);
void buscarRegistroYEscribirByte(uint16_t addr, uint8_t val);
void dmaOamTransferBegins();
void initMemory(char* gameROMFilePath);

 
// uint8_t readCartridge(uint16_t addr); en cartridge.c
uint8_t readVRAM(uint16_t addr);
uint8_t readWorkRam(uint16_t addr);
uint8_t readOAM(uint16_t addr);
uint8_t readRegister(uint16_t addr);
uint8_t readHRam(uint16_t addr);
uint8_t readInterruptRegisterControl();

// void writeCartridge(uint16_t addr, uint8_t val); en cartridge.c
void writeVRAM(uint16_t addr, uint8_t val);
void writeWorkRam(uint16_t addr, uint8_t val);
void writeOAM(uint16_t addr, uint8_t val);
void writeRegister(uint16_t addr, uint8_t val);
void writeHRam(uint16_t addr, uint8_t val);
void writeInterruptRegisterControl(uint8_t val);