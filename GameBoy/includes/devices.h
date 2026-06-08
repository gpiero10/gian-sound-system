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
#pragma once

#include "commons.h"
#include "cartridge.h"

#define cartridgeStart 0x0
#define cartridgeEnd 0x7FFF
#define cartridgeRange(x) (between(cartridgeStart, cartridgeEnd, x))
#define cartridgeId 0

#define vramStart 0x8000
#define vramEnd 0x9FFF
#define vramRange(x) (between(vramStart, vramEnd, x))
#define vramId 1

#define extRAMStart 0xA000
#define extRAMEnd 0xBFFF
#define extRAMRange(x) (between(extRAMStart, extRAMEnd, x))

#define workRAMStart 0xC000
#define workRAMEnd 0xDFFF
#define workRAMRange(x) (between(workRAMStart, workRAMEnd, x))
#define workRAMId 2

#define oamStart 0xFE00
#define oamEnd 0xFE9F
#define oamRange(x) (between(oamStart, oamEnd, x))
#define oamId 3

#define ioRegistersStart 0xFF00
#define ioRegistersEnd 0xFF7F
#define ioRegistersRange(x) (between(ioRegistersStart, ioRegistersEnd, x))
#define ioRegsId 4

#define hramStart 0xFF80
#define hramEnd 0xFFFE
#define hramRange(x) (between(hramStart, hramEnd, x))
#define hramId 5

#define ieRegisterAddr 0xFFFF
#define ieRegsId 6

#define cantDevices 7

typedef struct devices
{
    /* Interfaz de dispositivo */
    bool (*enRango)(u16); // Determina si un address pertenece al espacio de deirecciones asignado al device
    u8 (*read)(u16); // leer dato
    void (*write)(u16, u8); // escribir dato
    void* ctx; //contexto, struct del dispositivo especifico
} device_t;

typedef struct OAM
{
    // Tabla de 40 entradas de 4 bytes, donde cada entrada es un sprite
    uint8_t spritesTable[160];
} OAM_t;

typedef struct
{
    u8 data[eigthKB]; //(8 kib)
} vram_t;

typedef struct
{
    u8 data[eigthKB];
    u8 cantidadDeBancos; //Hasta 8 en cgb mode (0 y 1-7 switcheables)
    
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
    uint8_t interrupts; //  IF	Interrupt flag	R/W	All

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
    uint8_t registerControl; // $FFFF	IE	Interrupt enable	R/W	All
} IEregister_t;

void init_Devices(char* gameROMFilePath);

device_t* getDeviceByIndex(u8 i);
void dmaOamTransfer(u8 hAddr);
u8 getDMATransferState();
