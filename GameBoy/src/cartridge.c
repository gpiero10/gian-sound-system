#include "cartridge.h"

cartridge_t cartucho;

const License licenses[] = {
    {"00", "None"},
    {"01", "Nintendo R&D1"},
    {"08", "Capcom"},
    {"13", "Electronic Arts"},
    {"18", "Hudson Soft"},
    {"19", "B-AI"},
    {"20", "KSS"},
    {"22", "Planning Office WADA"},
    {"24", "PCM Complete"},
    {"25", "San-X"},
    {"28", "Kemco"},
    {"29", "SETA"},
    {"30", "Viacom"},
    {"31", "Nintendo"},
    {"32", "Bandai"},
    {"33", "Ocean/Acclaim"},
    {"34", "Konami"},
    {"35", "HectorSoft"},
    {"37", "Taito"},
    {"38", "Hudson Soft"},
    {"39", "Banpresto"},
    {"41", "Ubisoft"},
    {"42", "Atlus"},
    {"44", "Malibu"},
    {"46", "Angel"},
    {"47", "Bullet-Proof"},
    {"49", "Irem"},
    {"50", "Absolute"},
    {"51", "Acclaim"},
    {"52", "Activision"},
    {"53", "Sammy"},
    {"54", "Konami"},
    {"55", "Hi Tech"},
    {"56", "LJN"},
    {"57", "Matchbox"},
    {"58", "Mattel"},
    {"59", "Milton Bradley"},
    {"60", "Titus"},
    {"61", "Virgin"},
    {"64", "LucasArts"},
    {"67", "Ocean"},
    {"69", "EA"},
    {"70", "Infogrames"},
    {"71", "Interplay"},
    {"72", "Broderbund"},
    {"73", "Sculptured"},
    {"75", "Sales Curve"},
    {"78", "THQ"},
    {"79", "Accolade"},
    {"80", "Misawa"},
    {"83", "LOZC"},
    {"86", "Tokuma Shoten"},
    {"87", "Tsukuda Original"},
    {"91", "Chunsoft"},
    {"92", "Video System"},
    {"93", "Ocean/Acclaim"},
    {"95", "Varie"},
    {"96", "Yonezawa"},
    {"97", "Kaneko"},
    {"99", "Pack-In-Video"},
    {"A4", "Konami (Yu-Gi-Oh!)"},
    {"BL", "MTO"},
    {"DK", "Kodansha"}
};

const char* oldLicenses[256] = {
    [0x00] = "None",
    [0x01] = "Nintendo",
    [0x08] = "Capcom",
    [0x09] = "HOT-B",
    [0x0A] = "Jaleco",
    [0x0B] = "Coconuts Japan",
    [0x0C] = "Elite Systems",
    [0x13] = "Electronic Arts",
    [0x18] = "Hudson Soft",
    [0x19] = "ITC Entertainment",
    [0x1A] = "Yanoman",
    [0x1D] = "Japan Clary",
    [0x1F] = "Virgin Games",
    [0x24] = "PCM Complete",
    [0x25] = "San-X",
    [0x28] = "Kemco",
    [0x29] = "SETA",
    [0x30] = "Infogrames",
    [0x31] = "Nintendo",
    [0x32] = "Bandai",
    [0x33] = "Use New License Code",
    [0x34] = "Konami",
    [0x35] = "HectorSoft",
    [0x38] = "Capcom",
    [0x39] = "Banpresto",
    [0x3C] = "Entertainment Interactive",
    [0x3E] = "Gremlin",
    [0x41] = "Ubisoft",
    [0x42] = "Atlus",
    [0x44] = "Malibu",
    [0x46] = "Angel",
    [0x47] = "Spectrum Holobyte",
    [0x49] = "Irem",
    [0x4A] = "Virgin Games",
    [0x4D] = "Malibu",
    [0x4F] = "US Gold",
    [0x50] = "Absolute",
    [0x51] = "Acclaim",
    [0x52] = "Activision",
    [0x53] = "Sammy",
    [0x54] = "GameTek",
    [0x55] = "Park Place",
    [0x56] = "LJN",
    [0x57] = "Matchbox",
    [0x59] = "Milton Bradley",
    [0x5A] = "Mindscape",
    [0x5B] = "Romstar",
    [0x5C] = "Naxat Soft",
    [0x5D] = "Tradewest",
    [0x60] = "Titus",
    [0x61] = "Virgin Games",
    [0x67] = "Ocean",
    [0x69] = "Electronic Arts",
    [0x6E] = "Elite Systems",
    [0x6F] = "Electro Brain",
    [0x70] = "Infogrames",
    [0x71] = "Interplay",
    [0x72] = "Broderbund",
    [0x73] = "Sculptured Software",
    [0x75] = "Sales Curve",
    [0x78] = "THQ",
    [0x79] = "Accolade",
    [0x7A] = "Triffix",
    [0x7C] = "MicroProse",
    [0x7F] = "Kemco",
    [0x80] = "Misawa",
    [0x83] = "LOZC",
    [0x86] = "Tokuma Shoten",
    [0x8B] = "Bullet-Proof",
    [0x8C] = "Vic Tokai",
    [0x8E] = "Ape",
    [0x8F] = "I'Max",
    [0x91] = "Chunsoft",
    [0x92] = "Video System",
    [0x93] = "Tsubaraya",
    [0x95] = "Varie",
    [0x96] = "Yonezawa",
    [0x97] = "Kemco",
    [0x99] = "Arc",
    [0x9A] = "Nihon Bussan",
    [0x9B] = "Tecmo",
    [0x9C] = "Imagineer",
    [0x9D] = "Banpresto",
    [0x9F] = "Nova",
    [0xA1] = "Hori Electric",
    [0xA2] = "Bandai",
    [0xA4] = "Konami",
    [0xA6] = "Kawada",
    [0xA7] = "Takara",
    [0xA9] = "Technos Japan",
    [0xAA] = "Broderbund",
    [0xAC] = "Toei Animation",
    [0xAD] = "Toho",
    [0xAF] = "Namco",
    [0xB0] = "Acclaim",
    [0xB1] = "ASCII/Nexsoft",
    [0xB2] = "Bandai",
    [0xB4] = "Square Enix",
    [0xB6] = "HAL Laboratory",
    [0xB7] = "SNK",
    [0xB9] = "Pony Canyon",
    [0xBA] = "Culture Brain",
    [0xBB] = "Sunsoft",
    [0xBD] = "Sony Imagesoft",
    [0xBF] = "Sammy",
    [0xC0] = "Taito",
    [0xC2] = "Kemco",
    [0xC3] = "Square",
    [0xC4] = "Tokuma Shoten",
    [0xC5] = "Data East",
    [0xC6] = "Tonkin House",
    [0xC8] = "Koei",
    [0xC9] = "UFL",
    [0xCA] = "Ultra",
    [0xCB] = "VAP",
    [0xCC] = "Use",
    [0xCD] = "Meldac",
    [0xCE] = "Pony Canyon",
    [0xCF] = "Angel",
    [0xD0] = "Taito",
    [0xD1] = "SOFEL",
    [0xD2] = "Quest",
    [0xD3] = "Sigma Enterprises",
    [0xD4] = "ASK Kodansha",
    [0xD6] = "Naxat Soft",
    [0xD7] = "Copya System",
    [0xD9] = "Banpresto",
    [0xDA] = "Tomy",
    [0xDB] = "LJN",
    [0xDD] = "Nippon Computer Systems",
    [0xDE] = "Human",
    [0xDF] = "Altron",
    [0xE0] = "Jaleco",
    [0xE1] = "Towa Chiki",
    [0xE2] = "Yutaka",
    [0xE3] = "Varie",
    [0xE5] = "Epoch",
    [0xE7] = "Athena",
    [0xE8] = "Asmik Ace",
    [0xE9] = "Natsume",
    [0xEA] = "King Records",
    [0xEB] = "Atlus",
    [0xEC] = "Epic/Sony Records",
    [0xEE] = "IGS",
    [0xF0] = "A Wave",
    [0xF3] = "Extreme Entertainment",
    [0xFF] = "LJN"
};

const char* getNewPublisher(const char* code)
{
    int n = sizeof(licenses)/sizeof(licenses[0]);
    for (int i = 0; i < n; i++)
    {   
        if (strncmp(licenses[i].code, code, 2) == 0) 
        return licenses[i].publisher;
    }
    return "Unknown";
}

int headerRead(char* buffer, header_t* cabezal)
{
    memcpy(cabezal->title, &buffer[0x34], 16);
    memcpy(cabezal->manufacturerCode, &buffer[0x3f], 4);
    
    cabezal->cbgFlag = buffer[0x43];

    memcpy(cabezal->newLicenseCode, &buffer[0x44], 2);
    cabezal->newLicenseCode[2] = '\0';

    cabezal->sgbFlag = buffer[0x46];
    cabezal->cartridgeType = buffer[0x47];
    cabezal->romSize = buffer[0x48];
    cabezal->ramSize = buffer[0x49];
    cabezal->destinationCode = buffer[0x4a];
    cabezal->oldLicenseeCode = buffer[0x4b];
    cabezal->maskRomVersionNumb = buffer[0x4c];
    cabezal->headerCheck = buffer[0x4d];

    uint8_t checksum = 0;
    for (uint16_t address = 0x34; address <= 0x4C; address++)
    {
        checksum = checksum - buffer[address] - 1;
    }

    if (checksum != (uint8_t)cabezal->headerCheck) 
    {
        printf("error de check");
        return -1;
    }

    char* manufacturerCode;
    if (cabezal->oldLicenseeCode == 0x33)
    {
        manufacturerCode = getNewPublisher(cabezal->newLicenseCode);
    }
    else
    {
        manufacturerCode = oldLicenses[cabezal->oldLicenseeCode];
    }

    printf("Titulo: %.16s\n", cabezal->title);
    printf("Empresa: %s\n", manufacturerCode);

    return 0;
}

uint8_t cartridge(cartridge_t* cartucho)
{   
    FILE* rom = fopen(cartucho->gamePath, "rb");
    
    if (!rom) 
    {
        perror("Error abriendo ROM");
        return -1;
    }
    
    char buffer[80];
    //Lectura header del ""cartucho""" (.gb)
    fseek(rom, 0x0100, SEEK_SET);
    fread(buffer, 1, 80, rom);

    header_t cabezal;
    headerRead(buffer, &cabezal);
    cartucho->header = cabezal;
    cartucho->cantidadDeBancosROM = numberOfROMBanks(cartucho->header.romSize);
    cartucho->activeBankROM = 1; // Predeterminado el primero

    cartridgeTypeDefinitor(cartucho); // Defino que tipo de cartucho es
    cantidadDeBancosDeExtRAM(cartucho); // Bastante declarativo

    //Pido memo para todo el rom (.gb)
    uint32_t tamañoRom = romSizeCalc(cartucho->header.romSize);
    cartucho->romData = malloc(tamañoRom);
    
    //cargo el rom en la memo del struct que representa al cartucho
    fseek(rom, 0, SEEK_SET);
    fread(cartucho->romData, 1, tamañoRom, rom);
    fclose(rom);

    return 0;
}

void cantidadDeBancosDeExtRAM(cartridge_t *cartucho)
{
    // Code	SRAM size	Comment
    // $00	0	        No RAM
    // $01	–	        Unused 14
    // $02	8 KiB	    1 bank
    // $03	32 KiB	    4 banks of 8 KiB each
    // $04	128 KiB	    16 banks of 8 KiB each
    // $05	64 KiB	    8 banks of 8 KiB each

    switch (cartucho->header.ramSize)
    {
    case 0x00:
        cartucho->cantidadDeBancosDeRam = 0;
        break;
    case 0x02:
        cartucho->cantidadDeBancosDeRam = 1;
        break;
    case 0x03:
        cartucho->cantidadDeBancosDeRam = 4;
        break;
    case 0x04:
        cartucho->cantidadDeBancosDeRam = 16;
        break;
    case 0x05:
        cartucho->cantidadDeBancosDeRam = 8;
        break;
    default:
        break;
    }
}

void cartridgeTypeDefinitor(cartridge_t *cartucho)
{
    // Veo si hay Ram externa, MBC y demas
    // 0147 — Cartridge type
    // This byte indicates what kind of hardware is present on the cartridge — most notably its mapper.
    // Code	Type
    // $00	ROM ONLY
    // $01	MBC1
    // $02	MBC1+RAM
    // $03	MBC1+RAM+BATTERY
    // $08	ROM+RAM 11
    // $09	ROM+RAM+BATTERY 11
    // "ignoro el resto"
    // default

    cartucho->mbcPresent = 0;
    cartucho->externalRamPresent = 0;
    // casos
    switch (cartucho->header.cartridgeType)
    {
        case 0x00:
            break;

        case 0x01:
            cartucho->mbcPresent = 1;
            break;

        case 0x02:
            cartucho->mbcPresent = 1;
            cartucho->externalRamPresent = 1;
            break;

        case 0x03:
            cartucho->mbcPresent = 1;
            cartucho->externalRamPresent = 1;
            break;

        case 0x08:
            cartucho->externalRamPresent = 1;
            break;

        case 0x09:
            cartucho->externalRamPresent = 1;
            break;

        default:
            break;
    }
}

u8 readCartridge(uint16_t addr)
{
    if (between(0x0, 0x3FFF, addr)) //Banco 0
    {
        return cartucho.romData[addr];
    }
    else if(between(0x4000, 0x7FFF, addr))// Banco N ("intercambiable") 
    {
        // caso de cartucho con MBC
        uint16_t bankOffset = (cartucho.activeBankROM - 1)*(1<<14);
        return cartucho.romData[bankOffset + addr];
    }
    else if(between(0xA000, 0xBFFF, addr)) //dir extRam, if any (8kib)
    {
        if (cartucho.externalRamPresent == 1)
        {
            uint16_t bankOffset = (cartucho.activeBankRAM - 1)*(1<<13);
            return cartucho.ramData[bankOffset + addr - 0xA000];
        }
    }
    
    return -1;
}

void writeCartridge(uint16_t addr, uint8_t value)
{
    if (between(0x0, 0x7FFF, addr)) // Direccion de ROM
    {
        // No se puede escribir en ROM pero, se configura el MBC en caso de haberlo
    }
    else if(between(0xA000, 0xBFFF, addr)) //dir extRam, if any (8kib)
    {
        if (cartucho.externalRamPresent == 1)
        {
            u16 bankOffset = (cartucho.activeBankRAM - 1)*(1<<13);
            cartucho.romData[bankOffset + addr - 0xA000] = value;
        }
    }
    return;
}

cartridge_t* initCartridge(char* gameROM)
{
    memset(&cartucho, 0, sizeof(cartridge_t));
    cartucho.gamePath = gameROM;
    
    // cartridge(&cartucho); Original
    cartucho.romData = romTest; // TEST
    
    return &cartucho;
}
