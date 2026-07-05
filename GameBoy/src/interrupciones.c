#include <interrupciones.h>
#include <cpu.h>

void setInterruptFlag(interrupt_bit interrupt)
{   
    // FF0F — IF: Interrupt flag
    //       7	6	5	4	    3	    2	    1	0
    // IF		        Joypad	Serial	Timer	LCD	VBlank
    
    u8 interruptFlags = busRead(0xFF0F) | (1 << interrupt);
    busWrite(0xFF0F, interruptFlags);
}

void pushPC(cpu_context* ctx)
{
    // push pc
    u16 reg = readCPURegister(RT_PC);
    u8 high = byteAltoDeWord(reg);
    u8 low = byteBajoDeWord(reg);
    busWrite(--ctx->registers.sp, high);
    emu_cycles(1);
    busWrite(--ctx->registers.sp, low);
    emu_cycles(1);
}

void jumpToHandler(cpu_context* ctx, u16 addr, interrupt_bit interruptIndex)
{
    // Desactivo IME
    ctx->int_master_enabled = false;
    
    // Limpio el bit de IF correspondiente a la interrupcion atendida
    u8 interruptFlags = busRead(0xFF0F) & ~(1 << interruptIndex);
    busWrite(0xFF0F, interruptFlags);
    
    // Interrupt Service Routine
    emu_cycles(2);

    // pusheo PC
    pushPC(ctx);

    // Salto al handler Correspondiente
    ctx->registers.pc = addr;
    emu_cycles(1);
}

void intVBlankHandler(cpu_context* ctx) {jumpToHandler(ctx, 0x40, bit_vblank);}

void intLCDStatHandler(cpu_context *ctx){ jumpToHandler(ctx, 0x48, bit_stat);}

void intTimerHandler(cpu_context* ctx) {jumpToHandler(ctx, 0x50, bit_timer);}

void intSerialHandler(cpu_context* ctx){jumpToHandler(ctx, 0x58, bit_serial);}

void intJoypadHandler(cpu_context* ctx){jumpToHandler(ctx, 0x60, bit_joypad);}

void interruptCheck(cpu_context* ctx)
{
    // FFFF — IE: Interrupt enable
    //      7	6	5	4	    3	    2	    1	0
    // IE		        Joypad	Serial	Timer	LCD	VBlank
    
    // FF0F — IF: Interrupt flag
    //       7	6	5	4	    3	    2	    1	0
    // IF		        Joypad	Serial	Timer	LCD	VBlank
    if (ctx->int_master_enabled)
    {
        
        u8 registerIF = busRead(0xFF0F) & 0x1F;
        u8 registerIE = busRead(0xFFFF) & 0x1F;
        bool interruptProc = registerIF & registerIE ? true:false;

        if (interruptProc)
        {
            // Procedo a identificar las interrupciones a ser atendidas
            bool reqVBlank = bit0(registerIE) & bit0(registerIF) ? true : false;
            if (reqVBlank) {intVBlankHandler(ctx); return;}  // mas prioritario   

            bool reqLCD = bit1(registerIE) & bit1(registerIF) ? true : false;
            if (reqLCD) {intLCDStatHandler(ctx); return;}

            bool reqTimer = bit2(registerIE) & bit2(registerIF) ? true : false;
            if (reqTimer) {intTimerHandler(ctx); return;}

            bool reqSerial = bit3(registerIE) & bit3(registerIF) ? true : false;
            if (reqSerial) {intSerialHandler(ctx); return;}

            bool reqJoypad = bit4(registerIE) & bit4(registerIF) ? true : false;
            if (reqJoypad) {intJoypadHandler(ctx); return;}   // menos prioritario
          
        }
    }
    return;
}
