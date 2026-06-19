#include <cpu.h>

cpu_context cpu_ctx;
extern FILE* testLogFile;

u64 cantExec;

void testLog()
{
    //TEST Register LOG
    // printf(
    //     "PC=%04X OP=%02X\n"
    //     "FLAGS: N = %01X, Z = %01X, C = %01X, HC = %01X\n"
    //     "AF=%04X BC=%04X DE=%04X HL=%04X SP=%04X\n"
    //     "\n",
    //     readCPURegister(RT_PC),
    //     cpu_ctx.cur_opcode,
    //     getFlag(&cpu_ctx, F_N_Subtract),
    //     getFlag(&cpu_ctx, F_ZERO),
    //     getFlag(&cpu_ctx, F_CARRY),
    //     getFlag(&cpu_ctx, F_HalfCarry),
    //     readCPURegister(RT_AF),
    //     readCPURegister(RT_BC),
    //     readCPURegister(RT_DE),
    //     readCPURegister(RT_HL),
    //     readCPURegister(RT_SP)
    // );
    
    //File LOG
    u16 curPC = readCPURegister(RT_PC);
    fprintf(testLogFile,
        "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
        readCPURegister(RT_A),
        readCPURegister(RT_F),
        readCPURegister(RT_B),
        readCPURegister(RT_C),
        readCPURegister(RT_D),
        readCPURegister(RT_E),
        readCPURegister(RT_H),
        readCPURegister(RT_L),
        readCPURegister(RT_SP),
        curPC,
        busRead(curPC),
        busRead(curPC + 1),
        busRead(curPC + 2),
        busRead(curPC + 3)
    );
}
void noOP(){ int a = 67;}

void cpu_init()
{
    memset(&cpu_ctx, 0, sizeof(cpu_context));
    cpu_ctx.halted = false;
    cpu_ctx.activando_IME = false;
    cpu_ctx.delayPatriotico = -1;
    cpu_ctx.int_master_enabled = false;

    // Inicializacion Registros del cpu y del hardware (control, I/O, etc)
    cpu_registers registers;
    memset(&registers, 0, sizeof(cpu_registers));

    // Seteando el estado inicial de los registros en DMG
    registers.a = 0x01;
    registers.f = 0xB0;// 0x80; // Z=1, N=0, H=0, C=0
    registers.b = 0x00;
    registers.c = 0x13;
    registers.d = 0x00;
    registers.e = 0xD8;
    registers.h = 0x01;
    registers.l = 0x4D;
    registers.pc = 0x0100;
    registers.sp = 0xFFFE;

    cpu_ctx.registers = registers;
}

void cpuStep()
{
    // Decode
    cpu_ctx.cur_opcode = busRead(cpu_ctx.registers.pc++);
    if (cpu_ctx.cur_opcode != 0xCB)
    {        
        cpu_ctx.currentInstruction = instruction_by_opcode(cpu_ctx.cur_opcode);
    } 
    else
    {   
        cpu_ctx.cur_opcode = busRead(cpu_ctx.registers.pc++);
        cpu_ctx.currentInstruction = CB_instruction_by_opcode(cpu_ctx.cur_opcode);
    }
    
    // Fetch
    cpuFetch();

    // Execute
    in_proc processor = getProcessorForCurrentInst(&cpu_ctx);
    processor(&cpu_ctx);
    
}

void cpuHalted()
{
    u8 registerIF = busRead(0xFF0F) & 0x1F;
    u8 registerIE = busRead(0xFFFF) & 0x1F;
    bool interruptProc = registerIF & registerIE ? true:false;
    
    if (cpu_ctx.int_master_enabled)
    {
        while(!interruptProc)
        {
            registerIF = busRead(0xFF0F) & 0x1F;
            registerIE = busRead(0xFFFF) & 0x1F;
            interruptProc = registerIF & registerIE ? true:false;
        }
    
        interruptCheck(&cpu_ctx);
        // cpu_run ?
    }
    else
    {
        if (interruptProc)
        {
            // HALT BUG
        }
        else
        {
            while(!interruptProc)
            {
                registerIF = busRead(0xFF0F) & 0x1F;
                registerIE = busRead(0xFFFF) & 0x1F;
                interruptProc = registerIF & registerIE ? true:false;
            }
        }
        
        
    }
}

void cpuRun()
{   
    cantExec = 0;
    while (!cpu_ctx.halted)
    {   
        cantExec ++;
        testLog();
        if (cantExec == 31465) {/*hay un call a16 en 16461, en 16469 diverge mi emu*/ noOP();}

        cpuStep();  // Se ejecuta 1 instruccion
        
        if (cpu_ctx.activando_IME) 
        {
            if (cpu_ctx.delayPatriotico == 0)
            {
                cpu_ctx.int_master_enabled = true;
                cpu_ctx.activando_IME = false;
            } else
            {
                cpu_ctx.delayPatriotico --;
            }
        }

        interruptCheck(&cpu_ctx);

        // Leer de port SC (Tests de Blargg)
        if (busRead(0xFF02) == 0x81)
        {
            //noOP();
            putchar(busRead(0xFF01));
            busWrite(0xFF02, 0); //limpio el i/o port
        }

    }
    
    // Si se llega aca es porque la cpu se halteo
    //cpu_halted();
}
