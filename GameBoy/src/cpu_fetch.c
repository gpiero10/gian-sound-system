#include <cpu.h>
// Implementacion Antigua
extern cpu_context cpu_ctx;

void cpuFetch()
{
    // Aca cargamos la data que la instruccion necesite, en donde corresponda
    // valores predeterminados
    cpu_ctx.dest_is_mem = false;
    cpu_ctx.mem_dest = 0;

    switch (cpu_ctx.currentInstruction->mode)
    {
    case AM_IMP:
        break;

    case AM_R_D16:
    {
        u8 low = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        u8 high = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        cpu_ctx.fetched_data = ext16bits(high, low);
    }
    break;

    case AM_R_R:
        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
        break;

    case AM_MR_R:
        cpu_ctx.dest_is_mem = true;

        cpu_ctx.mem_dest = readCPURegister(cpu_ctx.currentInstruction->reg_1);

        if (cpu_ctx.currentInstruction->reg_1 == RT_C)
        {
            cpu_ctx.mem_dest |= 0xFF00;
        }

        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);

        break;

    case AM_R:
        break;

    case AM_R_D8:
        cpu_ctx.fetched_data = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        break;

    case AM_R_MR:
    {
        u16 addr = readCPURegister(cpu_ctx.currentInstruction->reg_2);

        if (cpu_ctx.currentInstruction->reg_2 == RT_C)
        {
            addr |= 0xFF00;
        }

        cpu_ctx.fetched_data = busRead(addr);
        emu_cycles(1); 
    }
    break;

    case AM_R_HLI:
    {
        u16 hl = readCPURegister(RT_HL);

        cpu_ctx.fetched_data = busRead(hl);
        emu_cycles(1); 

        writeCPURegister(RT_HL, hl + 1);
    }
    break;

    case AM_R_HLD:
    {
        u16 hl = readCPURegister(RT_HL);

        cpu_ctx.fetched_data = busRead(hl);
        emu_cycles(1); 

        writeCPURegister(RT_HL, hl - 1);
    }
    break;

    case AM_HLI_R:
    {
        u16 hl = readCPURegister(RT_HL);
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = hl;
        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
        writeCPURegister(RT_HL, hl + 1);
    }
    break;

    case AM_HLD_R:
    {
        u16 hl = readCPURegister(RT_HL);
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = hl;
        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
        writeCPURegister(RT_HL, hl - 1);
    }
    break;

    case AM_R_A8:
    {
        u16 addr = 0xFF00 | busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        cpu_ctx.fetched_data = busRead(addr);
        emu_cycles(1); 
    }
    break;

    case AM_A8_R:
    {
        cpu_ctx.dest_is_mem = true;

        cpu_ctx.mem_dest = 0xFF00 | busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
    }
    break;

    case AM_HL_SPR:
        cpu_ctx.fetched_data = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        break;

    case AM_D16:
    {
        u8 low = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 
        u8 high = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        cpu_ctx.fetched_data = ext16bits(high, low);
    }
    break;

    case AM_D8:
        cpu_ctx.fetched_data = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 
        break;

    case AM_D16_R:
    {
        u8 low = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 
        u8 high = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        cpu_ctx.fetched_data = ext16bits(high, low);
    }
    break;

    case AM_MR_D8:
    {
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = readCPURegister(cpu_ctx.currentInstruction->reg_1);

        cpu_ctx.fetched_data = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        break;
    }

    case AM_MR:
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = readCPURegister(cpu_ctx.currentInstruction->reg_1);

        cpu_ctx.fetched_data = busRead(cpu_ctx.mem_dest);
        emu_cycles(1); 
        break;

    case AM_A16_R:
    {
        cpu_ctx.dest_is_mem = true;

        u8 low = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 
        u8 high = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        cpu_ctx.mem_dest = ext16bits(high, low);
        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
    }
    break;

    case AM_R_A16:
        u8 low = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 
        u8 high = busRead(cpu_ctx.registers.pc++);
        emu_cycles(1); 

        cpu_ctx.fetched_data = busRead(ext16bits(high, low));
        emu_cycles(1); 
        break;

    default:
        break;
    }
}
