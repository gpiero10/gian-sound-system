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
        cpu_ctx.fetched_data = busRead16(cpu_ctx.registers.pc);
        cpu_ctx.registers.pc += 2;
        break;

    case AM_R_R:
        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
        break;

    case AM_MR_R:
        cpu_ctx.dest_is_mem = true;

        cpu_ctx.mem_dest = readCPURegister(cpu_ctx.currentInstruction->reg_1);

        if (between(RT_A, RT_L, cpu_ctx.currentInstruction->reg_1))
        {
            cpu_ctx.mem_dest |= 0xFF00;
        }

        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
        break;

    case AM_R:
        break;

    case AM_R_D8:
        cpu_ctx.fetched_data = busRead(cpu_ctx.registers.pc++);
        break;

    case AM_R_MR:
        {
            u16 addr = readCPURegister(cpu_ctx.currentInstruction->reg_2);

            if (between(RT_A, RT_L, cpu_ctx.currentInstruction->reg_2))
            {
                addr |= 0xFF00;
            }

            cpu_ctx.fetched_data = busRead(addr);
            break;
        }

    case AM_R_HLI:
        {
            u16 hl = readCPURegister(RT_HL);
            cpu_ctx.fetched_data = busRead(hl);
            writeCPURegister(RT_HL, hl + 1);
            break;
        }

    case AM_R_HLD:
        {
            u16 hl = readCPURegister(RT_HL);
            cpu_ctx.fetched_data = busRead(hl);
            writeCPURegister(RT_HL, hl - 1);
            break;
        }
    case AM_HLI_R:
        {
            u16 hl = readCPURegister(RT_HL);
            cpu_ctx.dest_is_mem = true;
            cpu_ctx.mem_dest = hl;
            cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
            writeCPURegister(RT_HL, hl + 1);
            break;
        }
    case AM_HLD_R:
        {
            u16 hl = readCPURegister(RT_HL);
            cpu_ctx.dest_is_mem = true;
            cpu_ctx.mem_dest = hl;
            cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
            writeCPURegister(RT_HL, hl - 1);
            break;
        }
    case AM_R_A8:
        {
            u16 addr = 0xFF00 | busRead(cpu_ctx.registers.pc++);
            cpu_ctx.fetched_data = busRead(addr);
            break;
        }
    case AM_A8_R:
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = 0xFF00 | busRead(cpu_ctx.registers.pc++);
        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
        break;

    case AM_HL_SPR:
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = readCPURegister(cpu_ctx.currentInstruction->reg_1);
        cpu_ctx.fetched_data = cpu_ctx.registers.sp;
        break;

    case AM_D16:
        cpu_ctx.fetched_data = busRead16(cpu_ctx.registers.pc);
        cpu_ctx.registers.pc += 2;
        break;

    case AM_D8:
        cpu_ctx.fetched_data = busRead(cpu_ctx.registers.pc++);
        break;

    case AM_D16_R:
        cpu_ctx.fetched_data = busRead16(cpu_ctx.registers.pc);
        cpu_ctx.registers.pc += 2;
        break;

    case AM_MR_D8:
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = readCPURegister(cpu_ctx.currentInstruction->reg_1);
        cpu_ctx.fetched_data = busRead(cpu_ctx.registers.pc++);
        break;

    case AM_MR:
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = readCPURegister(cpu_ctx.currentInstruction->reg_1);
        cpu_ctx.fetched_data = busRead(cpu_ctx.mem_dest);
        break;

    case AM_A16_R:
        cpu_ctx.dest_is_mem = true;
        cpu_ctx.mem_dest = busRead16(cpu_ctx.registers.pc);
        cpu_ctx.registers.pc += 2;
        cpu_ctx.fetched_data = readCPURegister(cpu_ctx.currentInstruction->reg_2);
        break;

    case AM_R_A16:
        cpu_ctx.fetched_data = busRead(busRead16(cpu_ctx.registers.pc));
        cpu_ctx.registers.pc += 2;
        break;

    default:
        break;
    }
}
