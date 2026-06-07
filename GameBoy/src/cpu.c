#include <cpu.h>

cpu_context cpu_ctx;

void cpu_init()
{
    memset(&cpu_ctx, 0, sizeof(cpu_context));
    cpu_ctx.halted = false;

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

    cpu_ctx.registers = registers;

}

void cpuStep()
{
    while (!cpu_ctx.halted)
    {
        // Decode
        cpu_ctx.cur_opcode = busRead(cpu_ctx.registers.pc++);
        cpu_ctx.currentInstruction = instruction_by_opcode(cpu_ctx.cur_opcode); 
        
        // Fetch
        cpuFetch();

        // Execute
        in_proc processor = getProcessorForCurrentInst(&cpu_ctx);
        processor(&cpu_ctx);
        // cpuProc();

    }
    
    // Si se llega aca es porque la cpu se halteo
    cpu_halted();

}

void cpu_halted()
{
    // TO DO...// 
    u8 ifRegister = busRead(0xFF0F); 
    u8 ieRegister = busRead(0xFFFF);
    u8 andi = ifRegister & ieRegister;
    while (andi == 0)
    {
        // Espero por alguna interrupcion
        ifRegister = busRead(0xFF0F); 
        ieRegister = busRead(0xFFFF);
        andi = ifRegister & ieRegister;
    }

    // si alguna interrupcion pide ser atendida y ademas se permite el handleo, se termina el halt
    cpu_ctx.halted = false;
    interrputHandling();

}

void interrputHandling() 
{
    printf("TO DO");
}