#include <cpu.h>

extern cpu_context cpu_ctx;

// Read/Write Registros de CPU
u16 readCPURegister(reg_type rt)
{
    u16 sourceRegister;
    switch (rt)
    {
    case RT_B:
        sourceRegister = cpu_ctx.registers.b;
        break;

    case RT_C:
        sourceRegister = cpu_ctx.registers.c;
        break;

    case RT_D:
        sourceRegister = cpu_ctx.registers.d;
        break;

    case RT_E:
        sourceRegister = cpu_ctx.registers.e;
        break;

    case RT_H:
        sourceRegister = cpu_ctx.registers.h;
        break;

    case RT_L:
        sourceRegister = cpu_ctx.registers.l;
        break;

    case RT_A:
        sourceRegister = cpu_ctx.registers.a;
        break;

    case RT_F:
        sourceRegister = cpu_ctx.registers.f;
        break;

    case RT_AF:
        sourceRegister = ext16bits(cpu_ctx.registers.a, cpu_ctx.registers.f);
        break;

    case RT_BC:
        sourceRegister = ext16bits(cpu_ctx.registers.b, cpu_ctx.registers.c);
        break;

    case RT_DE:
        sourceRegister = ext16bits(cpu_ctx.registers.d, cpu_ctx.registers.e);
        break;

    case RT_HL:
        sourceRegister = ext16bits(cpu_ctx.registers.h, cpu_ctx.registers.l);
        break;
    
    case RT_SP:
        sourceRegister = cpu_ctx.registers.sp;
        break;
    
    case RT_PC:
        sourceRegister = cpu_ctx.registers.pc;
        break;

    default:
        break;
    }
    return sourceRegister;
}

void writeCPURegister(reg_type dest, u16 val)
{
    switch (dest)
    {
    case RT_B:
        cpu_ctx.registers.b = val;
        break;

    case RT_C:
        cpu_ctx.registers.c = val;
        break;

    case RT_D:
        cpu_ctx.registers.d = val;
        break;

    case RT_E:
        cpu_ctx.registers.e = val;
        break;

    case RT_H:
        cpu_ctx.registers.h = val;
        break;

    case RT_L:
        cpu_ctx.registers.l = val;
        break;

    case RT_A:
        cpu_ctx.registers.a = val;
        break;
    
    case RT_F:
        cpu_ctx.registers.f = val;
        break;
    
    case RT_AF:
        writeCPURegister(RT_A, byteAltoDeWord(val));
        writeCPURegister(RT_F, byteBajoDeWord(val));
        break;

    case RT_BC:
        writeCPURegister(RT_B, byteAltoDeWord(val));
        writeCPURegister(RT_C, byteBajoDeWord(val));
        break;

    case RT_DE:
        writeCPURegister(RT_D, byteAltoDeWord(val));
        writeCPURegister(RT_E, byteBajoDeWord(val));
        break;

    case RT_HL:
        writeCPURegister(RT_H, byteAltoDeWord(val));
        writeCPURegister(RT_L, byteBajoDeWord(val));
        break;
    
    case RT_SP:
        cpu_ctx.registers.sp = val;
        break;

    case RT_PC:
        cpu_ctx.registers.pc = val;
        break;

    default:
        break;
    }
}
