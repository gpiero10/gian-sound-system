#include "commons.h"
#include "devices.h"

// Mapa de Memoria
// 0x0000–0x7FFF → ROM
// 0x8000–0x9FFF → VRAM
// 0xA000–0xBFFF → External RAM
// 0xC000–0xDFFF → Work RAM
// 0xFF00–0xFF7F → I/O Registers


u8 busRead(u16 addr);
u16 busRead16(u16 addr);
void busWrite(u16 addr, u8 val);
void busWrite16(u16 addr, u16 val);