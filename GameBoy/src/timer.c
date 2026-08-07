#include <timer.h>
static timer_t ctx;

const u16 clocks[] = {1024, 16, 64, 256};
const u8 shiftForBitSelectedByMode[] = {9, 3, 5, 7};

timer_t* init_Timer()
{
    ctx.systemCounter = 0xAB00;
    ctx.tima = 0;
    ctx.tma = 0;
    ctx.tac = 0xF8;
    ctx.overflowTCycles = -1;
    
    return &ctx;
}

// Incremento de registros Timer y un getter
void timerTick() {ctx.tima++;}
void incSystemCounter() {ctx.systemCounter++;}
u16 getCurrentTCycle() {return ctx.systemCounter;}

void timaOverflowRoutine()
{
    /*
    m-cyc	    X	Y	Z   A	B	C	​
    SYS	        00	04	08	0C	10	14	18
    TIMA	    FE	FF	FF	00	23	24	24
    TMA	        23	23	23	23	23	23	23
    IF	        E0	E0	E0	E0	E4	E4	E4
    */
    setInterruptFlag(bit_timer);   
    ctx.tima = ctx.tma; // TMA -> TIMA
}

void timerRegistersTick()
{
    incSystemCounter();
    ctx.overflowTCycles = max(-1, ctx.overflowTCycles - 1);
    
    u8 tima = ctx.tima; // TIMA: Timer counter
    u8 tma = ctx.tma;  // TMA: Timer modulo
    u8 tac = ctx.tac;  // TAC: Timer control
    u8 clockSelect = tac & 0x03; // TAC Mode
    bool enableTIMA = bit2(tac); // TAC Enable
    bool incTIMAFlag = ((getCurrentTCycle() % (clocks[clockSelect])) == 0) ? true : false;

    if (ctx.overflowTCycles == -1)
    {
        if (incTIMAFlag && enableTIMA)
        {
            u16 incTIMAValue = tima + 1;
            if (incTIMAValue > 0xFF) ctx.overflowTCycles = 8;
            ctx.tima = (u8)(incTIMAValue);
        }
    }
    else if (ctx.overflowTCycles > -1) 
    {
        if (ctx.overflowTCycles == 0) timaOverflowRoutine();
    }
}

// Funcion principal, actualiza el sistema en base a m-cycles
void emu_cycles(u8 mCycles)
{    
    for (size_t i = 0; i < mCycles; i++) 
    { 
        for (size_t j = 0; j < 4; j++) 
        {
            timerRegistersTick();
        }
    }
}

// Read and Write
void writeTimer(u16 addr, u8 val)
{
    u16 oldSystemCounter = ctx.systemCounter;
    u8 oldTac = ctx.tac;
    u8 oldTACEnable = bit2(oldTac);
    u8 oldTACmode = oldTac & 0x3; // bit selected by the multiplexor
    u8 oldSelectedBitOfSysCounter = (oldSystemCounter & (1 << shiftForBitSelectedByMode[oldTACmode])) >> shiftForBitSelectedByMode[oldTACmode];

    switch (addr)
    {
        case 0xFF04: 
            {
                //DIV
                // 1) Resetting the entire system counter (by writing to DIV) can reset the bit currently selected by the multiplexer, thus sending a “Timer tick” and/or “DIV-APU event” pulse early.
                ctx.systemCounter = 0;
                if (oldSelectedBitOfSysCounter == 1) timerTick();
            }

            break;

        case 0xFF05: 
            //TIMA
            // Here are some unexpected behaviors:
            // Writing to TIMA during cycle A acts as if the overflow didn’t happen! TMA will not be copied to TIMA (the value written will therefore stay), and bit 2 of IF will not be set. Writing to DIV, TAC, or other registers won’t prevent the IF flag from being set or TIMA from being reloaded.
            // Writing to TIMA during cycle B will be ignored; TIMA will be equal to TMA at the end of the cycle anyway.
            if (between(5,8,ctx.overflowTCycles))
            {
                ctx.overflowTCycles = -1;
            }
            if (!between(1,4,ctx.overflowTCycles))
            {
                ctx.tima = val;    
            }
            
            break;
        
        case 0xFF06: // TMA
            // Writing to TMA during cycle B will have the same value copied to TIMA as well, on the same cycle.
            ctx.tma = val;
            break;
        
        case 0xFF07: 
            {
                // TAC
                // 2) Changing which bit of the system counter is selected (by changing the “Clock select” bits of TAC) from a bit currently set to another that is currently unset, will send a “Timer tick” pulse. (For example: if the system counter is equal to $3FF0 and TAC to $FC, writing $05 or $06 to TAC will instantly send a “Timer tick”, but $04 or $07 won’t.)

                // 3) On monochrome consoles, disabling the timer if the currently selected bit is set, will send a “Timer tick” once. This does not happen on Color models.

                ctx.tac = val;
                u8 newEnable = bit2(ctx.tac);
                u8 newTACmode = ctx.tac & 0x3;
                u8 newSelectedBitOfSysCounter = (ctx.systemCounter & (1 << shiftForBitSelectedByMode[newTACmode])) >> shiftForBitSelectedByMode[newTACmode];

                if ((oldSelectedBitOfSysCounter == 1) && (newSelectedBitOfSysCounter == 0)) timerTick();

                if ((oldTACEnable == 1) && (newEnable == 0)) timerTick();
            }
            break;

        default:
            break;
    }
}
u8 readTimer(u16 addr)
{
    u8 dato;
    switch (addr)
    {
    case 0xFF04:
        dato = (u8)(ctx.systemCounter >> 8);
        break;
    case 0xFF05:
        dato = ctx.tima;
        break;
    case 0xFF06:
        dato = ctx.tma;
        break;
    case 0xFF07:
        dato = ctx.tac;
        break;

    default:
        break;
    }
    return dato;
}
