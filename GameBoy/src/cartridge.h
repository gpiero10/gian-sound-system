#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct
{
    char title[16];
    char manufacturerCode[4];
    char cbgFlag;
    char newLicenseCode[3];
    char sgbFlag;
    uint8_t cartridgeType;
    char romSize;
    char ramSize;
    char destinationCode;
    uint8_t oldLicenseeCode;
    char maskRomVersionNumb;
    char headerCheck;

} header_t;

typedef struct newLicense
{
    const char* code;
    const char* publisher;
} License;

typedef struct rom
{
    char* gamePath;
    header_t header;
    uint8_t cantidadDeBancos;
    char* data;
    
} rom_t;


extern const License licenses[];
extern const char* oldLicenses[256];

const char* getNewPublisher(const char* code);

int headerRead(char* buffer, header_t* cabezal);

uint8_t cartridge(rom_t* juego);