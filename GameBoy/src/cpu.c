#include "cpu.h"

cpu_context cpu_ctx;

void cpu_init()
{
    memset(&cpu_ctx, 0, sizeof(cpu_context));
    cpu_ctx.halt = false;

    // Inicializacion Registros del cpu y del hardware (control, I/O, etc)
    cpu_registers registers;
    memset(&registers, 0, sizeof(cpu_registers));
    
    // Seteando el estado inicial de los registros en DMG
    registers.a = 0x01;
    registers.f = 0x80; // Z=1, N=0, H=0, C=0
    registers.b = 0x00;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xD8;
    registers.h = 0x01;
    registers.l = 0x4D;
    registers.pc = 0x0100;
    registers.sp = 0xFFFE;

    // asumiendo que la memoria ya esta inicializada, escribo directamente aprovechando estas funciones
    writeRegister(0xFF00, 0xCF);
    writeRegister(0xFF01, 0x00);
    writeRegister(0xFF02, 0x7E);
    writeRegister(0xFF04, 0xAB);
    writeRegister(0xFF05, 0x00);

    writeRegister(0xFF06, 0x00);
    writeRegister(0xFF07, 0xF8);
    writeRegister(0xFF0F, 0xE1);
    writeRegister(0xFF10, 0x80);
    writeRegister(0xFF11, 0xBF);

    writeRegister(0xFF12, 0xF3);
    writeRegister(0xFF13, 0xFF);
    writeRegister(0xFF14, 0xBF);
    writeRegister(0xFF16, 0x3F);
    writeRegister(0xFF17, 0x00);

    writeRegister(0xFF18, 0xFF);
    writeRegister(0xFF19, 0xBF);
    writeRegister(0xFF1A, 0x7F);
    writeRegister(0xFF1B, 0xFF);
    writeRegister(0xFF1C, 0x9F);

    writeRegister(0xFF1D, 0xFF);
    writeRegister(0xFF1E, 0xBF);
    writeRegister(0xFF20, 0xFF);
    writeRegister(0xFF21, 0x00);
    writeRegister(0xFF22, 0x00);

    writeRegister(0xFF23, 0xBF);
    writeRegister(0xFF24, 0x77);
    writeRegister(0xFF25, 0xF3);
    writeRegister(0xFF26, 0xF1);
    writeRegister(0xFF40, 0x91);

    writeRegister(0xFF41, 0x85);
    writeRegister(0xFF42, 0x00);
    writeRegister(0xFF43, 0x00);
    writeRegister(0xFF44, 0x00);
    writeRegister(0xFF45, 0x00);

    writeRegister(0xFF46, 0xFF);
    writeRegister(0xFF47, 0xFC);
    //writeRegister(0xFF48, 0x00); undefined
    //writeRegister(0xFF49, 0x00); undefined
    writeRegister(0xFF4A, 0x00);
    writeRegister(0xFF4B, 0x00);

    cpu_ctx.registers = &registers;

}

void cpuCycle()
{
    // while (cpu not halted)
    // {
    //     fetch at pc
    //     decode
    //     fetchOperands
    //     execute

    //     pc inc/dec 
    // }

    //por ahora me enfoco en ld, nop y jp
    instruction_t instruction;
    
    while (!cpu_ctx.halt)
    {   
        instruction.romAddress = cpu_ctx.registers->pc;
        instruction.code = fetch(instruction.romAddress);
        cpu_ctx.currentInstruction = &instruction;
        decodeAndExecute(&instruction);
    }
} 

uint8_t fetch(uint16_t dir)
{
    return busRead(dir);
}

void decodeAndExecute()
{
    //Me quedo con los 2 bits mas altos (op code)
    (cpu_ctx.currentInstruction)->opCode = opCodeBits((cpu_ctx.currentInstruction)->code);
    u8 instruccion = (cpu_ctx.currentInstruction)->code;

    switch ((cpu_ctx.currentInstruction)->opCode)
    {
        case 0x0:
            // Caso bloque cero
            u8 parteBajaInst = mitadBajaByte(instruccion);
            if (instruccion == 0b00000000)
            {
                // nop	0	0	0	0	0	0	0	0
                nop();
            }
            else if(parteBajaInst == 0b0001)
            {
                u8 dest = bits4y5(instruccion);
                u16 imm16 = ext16bits(busRead(cpu_ctx.currentInstruction->romAddress + 1), 
                                    busRead(cpu_ctx.currentInstruction->romAddress + 2));
                // ld r16, imm16	0	0	Dest (r16)	0	0	0	1
                ld_R16_Imm16(dest, imm16);
            }
            else if(parteBajaInst == 0b0010)
            {
                ld_R16mem_A();
            }
            else if(parteBajaInst == 0b1010)
            {
                // ld a, [r16mem]	0	0	Source (r16mem)	1	0	1	0
                ld_A_R16mem();
            }
            else if(parteBajaInst == 0b1000)
            {
                ld_imm16_sp(); // ld [imm16], sp	0	0	0	0	1	0	0	0
            }
            else if(bits01y2(instruccion) == 0b110)
            {
                //ld r8, imm8	0	0	Dest (r8)	1	1	0
                ld_R8_imm8();
            }
            break;
        
        case 0x1:
            // Caso bloque uno
            uint8_t dest = (instruccion & 0x38) >> 3;
            uint8_t source = instruccion & 0x03;
            
            //Exception: trying to encode ld [hl], [hl] instead yields the halt instruction:
            if ((dest == 6) && (source == 6)) // lh = 0x6 (numero asociado al registro de cpu)
            {
                halt();
            }
            else
            {
                ld_R8_R8(dest, source);
            }
            
            break;
        case 0x2:
            // Caso bloque dos
            
            break;
        case 0x3:
            // Caso bloque tres
            uint8_t firstThreeBits = bits01y2(instruccion);
            if (firstThreeBits == 0x2)
            {
                // jp cond, imm16
                jp_Cond_Imm16();
            }
            else if (firstThreeBits == 0x3)
            {
                //jp imm16
                jp_imm16();
            } 
            else if (firstThreeBits == 0x1)
            {
                // jp hl	1	1	1	0	1	0	0	1
                if (instruccion == 0b11101001)
                {
                    // jp hl
                    jp_hl();
                }
            }
            break;
        
        default:
            break;
    }
}


// Instrucciones implementadas
/* NOP */
void nop()
{
    // no operation EL KVICHONAAAAAAAAAAAAAAAAAAAAAAAAAA
    return;
}

/* HALT */
void halt()
{
    cpu_ctx.halt = true;
}

/* LOAD */
void ld_R8_R8(u8 dest, u8 source)
{
    uint8_t srcValue = readCPURegister8ByIndex(source);
    writeCPURegister8ByIndex(dest, srcValue);
}
void ld_R16_Imm16(u8 dest, u16 imm16)
{
    // ID binario	ID decimal	Par de registros (rp, según contexto)
    // 00	        0	        BC
    // 01	        1	        DE
    // 10	        2	        HL
    // 11	        3	        SP (para LD rp, imm16, ADD HL, rp, INC rp, DEC rp), AF (solo para PUSH y POP)
    
    if (dest == 3)
    {
        writeCPURegister16ByIndex(SPr16id, imm16);
    }
    else
    {
        writeCPURegister16ByIndex(dest, imm16);
    }
}
void ld_R16mem_A()
{
    // ld [r16mem], a	0	0	Dest (r16mem)	0	0	1	0
    u8 dest = bits4y5((cpu_ctx.currentInstruction)->code);
    if (dest == HLplus)
    {
        busWrite(readCPURegister16ByIndex(HLr16id) + 1, readCPURegister8ByIndex(Ar8id));
    } 
    else if (dest == HLminus)
    {
        busWrite(readCPURegister16ByIndex(HLr16id) - 1, readCPURegister8ByIndex(Ar8id));
    }
    else
    {
        busWrite(readCPURegister16ByIndex(dest), readCPURegister8ByIndex(Ar8id));
    }
}
void ld_A_R16mem()
{
    u8 src = bits4y5(cpu_ctx.currentInstruction->code);
    if (src == HLplus)
    {
        writeCPURegister8ByIndex(Ar8id, busRead(readCPURegister16ByIndex(HLr16id) + 1));
        // es equivalente a ld a, [HL + 1] o ld a, [HLi]
    }
    else if (src == HLminus)
    {
        writeCPURegister8ByIndex(Ar8id, busRead(readCPURegister16ByIndex(HLr16id) - 1));
        // es equivalente a ld a, [HL - 1] o ld a, [HLd]
    }
    else
    {
        writeCPURegister8ByIndex(Ar8id, busRead(readCPURegister16ByIndex(src)));
    }
}
void ld_imm16_sp()
{
    // ld [imm16], sp	0	0	0	0	1	0	0	0
    u16 imm16 = ext16bits(busRead(cpu_ctx.currentInstruction->romAddress + 1),
                            busRead(cpu_ctx.currentInstruction->romAddress + 2));

    // En little endian
    busWrite(imm16, byteAltoDeWord(readCPURegister16ByIndex(SPr16id)));
    busWrite(imm16 + 1, byteBajoDeWord(readCPURegister16ByIndex(SPr16id)));
}
void ld_R8_imm8()
{
    //ld r8, imm8	0	0	Dest (r8)	1	1	0
    u8 imm8 = busRead(cpu_ctx.currentInstruction->romAddress + 1);
    u8 destR8 = bits345(cpu_ctx.currentInstruction->code);
    writeCPURegister8ByIndex(destR8, imm8);
}

/* JP */
void jp_Cond_Imm16()
{
    // jp cond, imm16
    // ¿Qué es cond?
    //  Son 2 bits que seleccionan una de 4 condiciones posibles:
    //  00 → NZ (Not Zero)
    //  01 → Z  (Zero)
    //  10 → NC (Not Carry)
    //  11 → C  (Carry)
    u16 imm16 = ext16bits(busRead((cpu_ctx.currentInstruction)->romAddress + 1),busRead((cpu_ctx.currentInstruction)->romAddress + 2));
    u8 condition = bits3y4(cpu_ctx.currentInstruction->code);
    u8 zeroFlag = (cpu_ctx.registers->f & 0x80) >> 7;
    u8 carryFlag = (cpu_ctx.registers->f & 0x10) >> 4;

    if (condition == 0x0)
    {
        // Cond = 00 (Not Zero)
        if (!zeroFlag)
        {
            // Si el flag de zero esta desactivado, saltamos
            cpu_ctx.registers->pc = imm16;
        }
    }
    else if (condition == 0x1)
    {
        // Cond = 01 (Zero)
        if (zeroFlag)
        {
            // Si el flag de zero esta activado, saltamos
            cpu_ctx.registers->pc = imm16;
        }
    }
    else if (condition == 0x2)
    {
        // Cond = 10 (Not Carry)
        if (!carryFlag)
        {
            // Si el flag de zero esta activado, saltamos
            cpu_ctx.registers->pc = imm16;
        }
    }
    else if (condition == 0x3)
    {
        // Cond = 11 (Carry)
        if (carryFlag)
        {
            // Si el flag de zero esta activado, saltamos
            cpu_ctx.registers->pc = imm16;
        }
    }
}
void jp_imm16()
{
    u16 imm16 = ext16bits(busRead((cpu_ctx.currentInstruction)->romAddress + 1),busRead((cpu_ctx.currentInstruction)->romAddress + 2));
    cpu_ctx.registers->pc = imm16;
}
void jp_hl()
{
    cpu_ctx.registers->pc = readCPURegister16ByIndex(HLr16id);
}

// Read/Write Registros de CPU
u8 readCPURegister8ByIndex(u8 id)
{
    u8 sourceRegister;
    switch (id)
    {
    case Br8id:
        sourceRegister = cpu_ctx.registers->b;
        break;

    case Cr8id:
        sourceRegister = cpu_ctx.registers->c;
        break;

    case Dr8id:
        sourceRegister = cpu_ctx.registers->d;
        break;

    case Er8id:
        sourceRegister = cpu_ctx.registers->e;
        break;

    case Hr8id:
        sourceRegister = cpu_ctx.registers->h;
        break;

    case Lr8id:
        sourceRegister = cpu_ctx.registers->l;
        break;

    case 0x6:
        // uint16_t specialCase;
        // specialCase = (cpu_ctx.registers->h << 8) | cpu_ctx.registers->l;
        // break;

    case Ar8id:
        sourceRegister = cpu_ctx.registers->a;
        break;

    default:
        break;
    }
    return sourceRegister;
}
u16 readCPURegister16ByIndex(u8 id)
{
    u16 sourceRegister;
    switch (id)
    {
    case AFr16id:
        sourceRegister = ext16bits(cpu_ctx.registers->a, cpu_ctx.registers->f);
        break;

    case BCr16id:
        sourceRegister = ext16bits(cpu_ctx.registers->b, cpu_ctx.registers->c);
        break;

    case DEr16id:
        sourceRegister = ext16bits(cpu_ctx.registers->d, cpu_ctx.registers->e);
        break;

    case HLr16id:
        sourceRegister = ext16bits(cpu_ctx.registers->h, cpu_ctx.registers->l);
        break;
    
    case SPr16id:
        sourceRegister = cpu_ctx.registers->sp;
        break;
    
    case PCr16id:
        sourceRegister = cpu_ctx.registers->pc;
        break;
    
    default:
        break;
    }
    return sourceRegister;
}

void writeCPURegister8ByIndex(u8 dest, u8 val)
{
    
    switch (dest)
    {
    case Br8id:
        cpu_ctx.registers->b = val;
        break;

    case Cr8id:
        cpu_ctx.registers->c = val;
        break;

    case Dr8id:
        cpu_ctx.registers->d = val;
        break;

    case Er8id:
        cpu_ctx.registers->e = val;
        break;

    case Hr8id:
        cpu_ctx.registers->h = val;
        break;

    case Lr8id:
        cpu_ctx.registers->l = val;
        break;

    case 0x6:
        // uint16_t specialCase;
        // specialCase = (registers.h << 8) | registers.l;
        break;

    case Ar8id:
        cpu_ctx.registers->a = val;
        break;

    default:
        break;
    }
}
void writeCPURegister16ByIndex(u8 id, u16 val)
{
    switch (id)
    {
    case AFr16id:
        writeCPURegister8ByIndex(Ar8id, byteAltoDeWord(val));
        cpu_ctx.registers->f = byteBajoDeWord(val);
        break;

    case BCr16id:
        writeCPURegister8ByIndex(Br8id, byteAltoDeWord(val));
        writeCPURegister8ByIndex(Cr8id, byteBajoDeWord(val));
        break;

    case DEr16id:
        writeCPURegister8ByIndex(Dr8id, byteAltoDeWord(val));
        writeCPURegister8ByIndex(Er8id, byteBajoDeWord(val));
        break;

    case HLr16id:
        writeCPURegister8ByIndex(Hr8id, byteAltoDeWord(val));
        writeCPURegister8ByIndex(Lr8id, byteBajoDeWord(val));
        break;
    
    case SPr16id:
        cpu_ctx.registers->sp = val;
    
    case PCr16id:
        cpu_ctx.registers->pc = val;

    default:
        break;
    }
}