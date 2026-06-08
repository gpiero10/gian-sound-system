#pragma once

#include "commons.h"
#define romSizeCalc(x) ((1 <<15) << x)
#define numberOfROMBanks(x) (1 << x)

typedef struct
{
    char title[16]; 
    char manufacturerCode[4]; // unknown
    char cbgFlag; // lo ignoro, hago dmg (monocromatico)
    char newLicenseCode[3]; // indica empresa pero actualizado
    char sgbFlag; // superGameBoy, es un adaptador para SNES.
    uint8_t cartridgeType; // indica los componentes del cartucho (si tiene ram, bateria, mbc)
    char romSize;   // bastante declarativo, 32kib << x
    char ramSize;   // Indica tamaño de la ram (si hay), 0x00 <= x <= 0x05 cada valor indica el tamaño aunque no es intuitivo (ver tabla)
    char destinationCode; // continente al que se exporta ??
    uint8_t oldLicenseeCode; // indica empresa del juego
    char maskRomVersionNumb; // indica version del juego, jajaja. Es 0x00
    char headerCheck; // indica un verificador adicional de originalidad

} header_t;

typedef struct newLicense
{
    const char* code;
    const char* publisher;
} License;

typedef struct Cartucho
{
    char* gamePath;
    header_t header;
    uint8_t cantidadDeBancosROM;
    uint8_t activeBankROM;
    char* romData;

    uint8_t externalRamPresent; // 1 o 0 dependiendo del header.cartridgetype
    uint8_t cantidadDeBancosDeRam;
    uint8_t activeBankRAM;
    char* ramData; //if any

    uint8_t mbcPresent; // 1 o 0 dependiendo del header.cartridgetype
} cartridge_t;

uint8_t readCartridge(uint16_t addr);
void writeCartridge(uint16_t addr, uint8_t value);
cartridge_t* initCartridge(char* gameROM);

extern u8 romTest[0x8000];