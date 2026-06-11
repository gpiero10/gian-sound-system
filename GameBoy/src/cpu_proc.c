#include <cpu.h>

// Utilidades para los processors
static bool is16BitsRegister(reg_type rt) {return rt >= RT_AF;}
static bool is8BitsRegister(reg_type rt) {return between(RT_A, RT_L, rt);}

void setFlag(cpu_context* ctx, flag_t f, i8 val)
{
    // Bit	Name	Explanation
    // 7	z	Zero flag
    // 6	n	Subtraction flag (BCD)
    // 5	h	Half Carry flag (BCD)
    // 4	c	Carry flag    
    if (val < 0) return;
    else if (val == 1) 
    {
        switch (f)
        {
        case F_CARRY:
            ctx->registers.f |=  0x10;
            break;

        case F_HalfCarry:
            ctx->registers.f |=  0x20;
            break;

        case F_N_Subtract:
            ctx->registers.f |=  0x40;
            break;

        case F_ZERO:
            ctx->registers.f |=  0x80;
            break;    
        default:
            break;
        }
    } 
    else if (val == 0)
    {
        switch (f)
        {
        case F_CARRY:
            ctx->registers.f &=  0b11101111;
            break;

        case F_HalfCarry:
            ctx->registers.f &=  0b11011111;
            break;

        case F_N_Subtract:
            ctx->registers.f &=  0b10111111;
            break;

        case F_ZERO:
            ctx->registers.f &=  0b01111111;
            break;

        default:
            break;
        }
    }
}
u8 getFlag(cpu_context* ctx, flag_t f)
{
    u8 fRegister = ctx->registers.f;
    switch (f)
    {
    case F_CARRY:
        return bitFlagCarry(fRegister);
        
    case F_HalfCarry:
        return bitFlagHalfCarry(fRegister);

    case F_N_Subtract:
        return bitFlagSubtraction(fRegister);
        
    case F_ZERO:
        return bitFlagZero(fRegister);
    
    default:
        break;
    }
    return -1;
}
void setFlags(cpu_context* ctx, u8 valCarry, u8 valHalfCarry, u8 valNSubtraction, u8 valZero)
{
    setFlag(ctx, F_CARRY, valCarry);
    setFlag(ctx, F_HalfCarry, valHalfCarry);
    setFlag(ctx, F_N_Subtract, valNSubtraction);
    setFlag(ctx, F_ZERO, valZero);
}

void checkHalfCarryFlag8Bits(u8 sum1, u8 sum2, cpu_context * ctx)
{
    u8 halfCarryFlag = ((sum1 & 0xF) + (sum2 & 0xF)) > 0xF ? 1:0;
    setFlag(ctx, F_HalfCarry, halfCarryFlag);
}
void checkZeroFlag8Bits(u8 suma, cpu_context *ctx)
{
    u8 zeroFlag = suma == 0 ? 1 : 0;
    setFlag(ctx, F_ZERO, zeroFlag);
}
void checkCarryFlag8Bits(u16 suma, cpu_context *ctx)
{
    u8 carryFlag = suma > 0xFF ? 1 : 0;
    setFlag(ctx, F_CARRY, carryFlag);
}
void checkHalfBorrow8Bits(u8 src, u8 minuendo, cpu_context *ctx)
{
    u8 halfBorrow = ((src & 0xF) < (minuendo & 0xF)) ? 1 : 0;
    setFlag(ctx, F_HalfCarry, halfBorrow);
}
void checkBorrow8Bits(u8 src, u8 minuendo, cpu_context *ctx)
{
    u8 borrow = (src < minuendo) ? 1 : 0;
    setFlag(ctx, F_CARRY, borrow);
}
// Fin Utilidades

// Procesors/Implementaciones de Instrucciones
void proc_nop(cpu_context* ctx) {} //NoOpp

void proc_ld(cpu_context* ctx)
{
    // ld dst src
    if (ctx->dest_is_mem)
    {
        if (is16BitsRegister(ctx->currentInstruction->reg_2))
        {
            busWrite16(ctx->mem_dest, ctx->fetched_data);
        }
        else
        {
            busWrite(ctx->mem_dest, ctx->fetched_data);
        }   
    }
    else
    {
        writeCPURegister(ctx->currentInstruction->reg_1, ctx->fetched_data);   
    }   
    // No flags affected
}

void proc_jr(cpu_context* ctx)
{
    i8 relativeJump = (i8)ctx->fetched_data;
    switch (ctx->currentInstruction->cond)
    {
        case CT_NONE:
            ctx->registers.pc += relativeJump;
            break;

        case CT_C:
            if (bitFlagCarry(ctx->registers.f))
            {
                ctx->registers.pc += relativeJump;
            }
            break;

        case CT_NC:
            if (!bitFlagCarry(ctx->registers.f))
            {
                ctx->registers.pc += relativeJump;
            }
            break;

        case CT_Z:
            if (bitFlagZero(ctx->registers.f))
            {
                ctx->registers.pc += relativeJump;
            }
            break;

        case CT_NZ:
            if (!bitFlagZero(ctx->registers.f))
            {
                ctx->registers.pc += relativeJump;
            }
            break;

        default:
            break;
    }
}

void proc_jp(cpu_context* ctx)
{
    // JP n16
    // JP cc,n16
    // JP HL

    switch (ctx->currentInstruction->type)
    {
        case IN_JP:
            switch (ctx->currentInstruction->cond)
            {
            case CT_NONE:
                ctx->registers.pc = ctx->fetched_data;
                break;
            
            case CT_C:
                if (getFlag(ctx, F_CARRY))
                {
                    ctx->registers.pc = ctx->fetched_data;
                }
                break;
            
            case CT_NC:
                if (!getFlag(ctx, F_CARRY))
                {
                    ctx->registers.pc = ctx->fetched_data;
                }
                break;

            case CT_Z:
                if (getFlag(ctx, F_ZERO))
                {
                    ctx->registers.pc = ctx->fetched_data;
                }
                break;

            case CT_NZ:
                if (!getFlag(ctx, F_ZERO))
                {
                    ctx->registers.pc = ctx->fetched_data;
                }
                break;

            default:
                break;
            }
            break; 
        
        case IN_JPHL:
            ctx->registers.pc = readCPURegister(ctx->currentInstruction->reg_1);
            break;
        
        default: 
            break;
    }
    
    // NO flags affected 
    return;
}

void proc_inc(cpu_context* ctx)
{
    // INC r8, INC [HL], INC r16, INC SP
    u16 val;
    u16 src;
    if (ctx->dest_is_mem)
    {
        src = busRead(ctx->mem_dest); 
        val = src + 1;
        busWrite(ctx->mem_dest, (u8)val);
    }
    else
    {
        src = readCPURegister(ctx->currentInstruction->reg_1);
        val = src + 1;
        if (is16BitsRegister(ctx->currentInstruction->reg_1))
        {
            writeCPURegister(ctx->currentInstruction->reg_1, val);
        }
        else
        {
            writeCPURegister(ctx->currentInstruction->reg_1, (u8)val);
        }
    }
    
    // Flags
    if (!is16BitsRegister(ctx->currentInstruction->reg_1) || ctx->dest_is_mem)
    {
        checkZeroFlag8Bits(val, ctx);
        setFlag(ctx, F_N_Subtract, 0);
        checkHalfCarryFlag8Bits((u8)src, 1, ctx);
    }
}

void proc_dec(cpu_context* ctx)
{
    // DEC r8
    // DEC [HL]
    // DEC r16
    // DEC SP
    
    u16 src;
    u16 val;
    if (ctx->dest_is_mem)
    {
        src = busRead(ctx->mem_dest);
        val = src - 1; 
        busWrite(ctx->mem_dest,  val);
    }
    else
    {
        src = readCPURegister(ctx->currentInstruction->reg_1);
        val = src - 1; 
        writeCPURegister(ctx->currentInstruction->reg_1, val);
    }

    // Flags
    if (!is16BitsRegister(ctx->currentInstruction->reg_1) || ctx->dest_is_mem)
    {
        // En caso DEC [HL] y DEC r8 si hay chequeo de flags
        checkZeroFlag8Bits(val, ctx);
        checkHalfBorrow8Bits(src, 1, ctx);
        setFlag(ctx, F_N_Subtract, 1);
    }
}

void proc_rlca(cpu_context *ctx)
{
    //  Rotate register A left.
    // ┏━ Flags ━┓   ┏━━━━━━━ A ━━━━━━━┓
    // ┃    C   ←╂─┬─╂─ b7 ← ... ← b0 ←╂─┐
    // ┗━━━━━━━━━┛ │ ┗━━━━━━━━━━━━━━━━━┛ │
    //             └─────────────────────┘

    u8 bit7 = bit7(ctx->registers.a);
    ctx->registers.a = (ctx->registers.a << 1) | bit7;

    setFlags(ctx, bit7, 0, 0, 0);
}

void proc_add(cpu_context* ctx)
{ 
    // ADD A,r8
    // ADD A,[HL]
    // ADD A,n8
    // ADD HL,r16
    // ADD HL,SP
    // ADD SP,e8
    
    u16 dstVal = readCPURegister(ctx->currentInstruction->reg_1);
    u16 suma;
    
    if (ctx->currentInstruction->reg_1 == RT_SP)
    { suma = dstVal + (i8)ctx->fetched_data; }
    else { suma = dstVal + ctx->fetched_data; }
    
    writeCPURegister(ctx->currentInstruction->reg_1, suma);

    // FLAGS
    if (is16BitsRegister(ctx->currentInstruction->reg_1))
    {
        switch (ctx->currentInstruction->reg_1)
        {
        case RT_HL:
            // Flags:
            // N   0
            // H   Set if overflow from bit 11.
            // C   Set if overflow from bit 15.
            u8 halfCarry = (((dstVal & 0xFFF) + (ctx->fetched_data & 0xFFF)) > 0xFFF) ? 1:0;
            setFlag(ctx, F_HalfCarry, halfCarry);
            
            u8 carryWord = (((u32)dstVal + (u32)ctx->fetched_data) > 0xFFFF) ? 1:0;
            setFlag(ctx, F_CARRY, carryWord);

            break;
        
        case RT_SP:
            // Flags:
            // Z   0
            // N   0
            // H   Set if overflow from bit 3.
            // C   Set if overflow from bit 7.

            // flags
            checkCarryFlag8Bits(suma, ctx);
            checkHalfCarryFlag8Bits(dstVal, (i8)ctx->fetched_data, ctx);

            setFlag(ctx, F_ZERO, 0);
            break;
        
        default:
            break;
        }
    }
    else
    {
        // flags
        checkZeroFlag8Bits(suma, ctx);
        checkCarryFlag8Bits(suma, ctx);
        checkHalfCarryFlag8Bits(dstVal, ctx->fetched_data, ctx);
    }
    
   setFlag(ctx, F_N_Subtract, 0);
}

void proc_sub(cpu_context* ctx)
{
    u8 dst = readCPURegister(ctx->currentInstruction->reg_1);
    u8 resta = dst - ctx->fetched_data;
    writeCPURegister(ctx->currentInstruction->reg_1, resta);

    //FLAGS
    checkZeroFlag8Bits(resta, ctx);
    checkBorrow8Bits(dst, ctx->fetched_data, ctx);
    checkHalfBorrow8Bits(dst, ctx->fetched_data, ctx);
    setFlag(ctx, F_N_Subtract, 1);
}

void proc_rrca(cpu_context* ctx)
{
    //     Rotate register A right.
    //   ┏━━━━━━━ A ━━━━━━━┓   ┏━ Flags ━┓
    // ┌─╂→ b7 → ... → b0 ─╂─┬─╂→   C    ┃
    // │ ┗━━━━━━━━━━━━━━━━━┛ │ ┗━━━━━━━━━┛
    // └─────────────────────┘

    u8 bit0 = 0x01 & ctx->registers.a;
    ctx->registers.a = (ctx->registers.a >> 1) | (bit0 << 7);
    setFlags(ctx, bit0, 0, 0, 0);
}

void proc_stop(cpu_context* ctx)
{
    // Enter CPU very low power mode. Also used to switch between GBC double speed and normal speed CPU modes.
    // The exact behavior of this instruction is fragile and may interpret its second byte as a separate instruction (see the Pan Docs), which is why rgbasm(1) allows explicitly specifying the second byte (STOP n8) to override the default of $00 (a NOP instruction).
    // Cycles: -
    // Bytes: 2
    // Flags: None affected.
    

}

void proc_rla(cpu_context* ctx)
{
    //Rotate register A left, through the carry flag.
    //   ┏━ Flags ━┓ ┏━━━━━━━ A ━━━━━━━┓
    // ┌─╂─   C   ←╂─╂─ b7 ← ... ← b0 ←╂─┐
    // │ ┗━━━━━━━━━┛ ┗━━━━━━━━━━━━━━━━━┛ │
    // └─────────────────────────────────┘
    u8 bitCarry = getFlag(ctx, F_CARRY);
    u8 bit7 = bit7(ctx->registers.a);
    ctx->registers.a = ctx->registers.a << 1 | bitCarry;

    setFlags(ctx, bit7, 0, 0, 0);
}

void proc_rra(cpu_context* ctx)
{
    // Rotate register A right, through the carry flag.
    //       ┏━━━━━━━ A ━━━━━━━┓ ┏━ Flags ━┓
    //     ┌─╂→ b7 → ... → b0 ─╂─╂→   C   ─╂─┐
    //     │ ┗━━━━━━━━━━━━━━━━━┛ ┗━━━━━━━━━┛ │
    //     └─────────────────────────────────┘

    u8 bitCarry = getFlag(ctx, F_CARRY);
    u8 bit0 = ctx->registers.a & 0x1;
    ctx->registers.a = ctx->registers.a >> 1 | (bitCarry << 7);

    setFlags(ctx, bit0, 0, 0, 0);
}

void proc_daa(cpu_context* ctx)
{
    // Decimal Adjust Accumulator.
    // Designed to be used after performing an arithmetic instruction (ADD, ADC, SUB, SBC) whose inputs were in Binary-Coded Decimal (BCD), adjusting the result to likewise be in BCD.
    // The exact behavior of this instruction depends on the state of the subtract flag N:
    // If the subtract flag N is set:
    //     Initialize the adjustment to 0.
    //     If the half-carry flag H is set, then add $6 to the adjustment.
    //     If the carry flag is set, then add $60 to the adjustment.
    //     Subtract the adjustment from A.
    // If the subtract flag N is not set:
    //     Initialize the adjustment to 0.
    //     If the half-carry flag H is set or A & $F > $9, then add $6 to the adjustment.
    //     If the carry flag is set or A > $99, then add $60 to the adjustment and set the carry flag.
    //     Add the adjustment to A.
    // Cycles: 1
    // Bytes: 1
    // Flags:
    // Z   Set if result is 0.
    // H   0
    // C   Set or unaffected depending on the operation.
    u8 regA = readCPURegister(RT_A);
    u8 nFlag = getFlag(ctx, F_N_Subtract);
    u8 cFlag = getFlag(ctx, F_CARRY);
    u8 hcFlag = getFlag(ctx, F_HalfCarry);
    u8 adjustement = 0;
    u8 newA;
    if (nFlag)
    {
        if (hcFlag) {adjustement += 0x6;}
        if (cFlag) {adjustement += 0x60;}
        newA = regA - adjustement;
        writeCPURegister(RT_A, newA);
    }
    else
    {

        if (hcFlag || ((regA & 0xF) > 0x9)) {adjustement += 0x6;}
        if (cFlag || (regA > 0x99)) {adjustement += 0x60; setFlag(ctx, F_CARRY, 1);}
        newA = regA + adjustement;
        writeCPURegister(RT_A, newA);
    }
    setFlags(ctx, -1, 0, -1, newA == 0 ? 1:0);
}

void proc_cpl(cpu_context* ctx)
{
    // CPL
    // ComPLement accumulator (A = ~A); also called bitwise NOT.
    // Cycles: 1
    // Bytes: 1
    // Flags:
    // N    1
    // H    1
    u8 notA = ~(readCPURegister(RT_A));
    writeCPURegister(RT_A, notA);

    setFlags(ctx, -1, 1, 1, -1);
}

void proc_scf(cpu_context* ctx)
{
    // Set Carry Flag.
    // Cycles: 1
    // Bytes: 1
    // Flags:
    // N    0
    // H    0
    // C    1
    
    // setFlag(ctx, F_CARRY, 1);
    // setFlag(ctx, F_HalfCarry, 0);
    // setFlag(ctx, F_N_Subtract, 0);
    setFlags(ctx, 1, 0, 0, -1); // -1 means it the flag reamins untouched
}

void proc_ccf(cpu_context* ctx)
{
    // Complement Carry Flag.
    // Cycles: 1
    // Bytes: 1
    // Flags:
    // N    0
    // H    0
    // C    Inverted.
    u8 notCarry = !(getFlag(ctx, F_CARRY));
    setFlags(ctx, notCarry, 0, 0, -1);
}

void proc_halt(cpu_context* ctx)
{
    // Enter CPU low-power consumption mode until an interrupt occurs.
    // The exact behavior of this instruction depends on the state of the IME flag, and whether interrupts are pending (i.e. whether ‘[IE] & [IF]’ is non-zero):
    // If the IME flag is set:
    // The CPU enters low-power mode until after an interrupt is about to be serviced. The handler is executed normally, and the CPU resumes execution after the HALT when that returns.
    // If the IME flag is not set, and no interrupts are pending:
    // As soon as an interrupt becomes pending, the CPU resumes execution. This is like the above, except that the handler is not called.
    // If the IME flag is not set, and some interrupt is pending:
    // The CPU continues execution after the HALT, but the byte after it is read twice in a row (PC is not incremented, due to a hardware bug).
    // Cycles: -
    // Bytes: 1
    // Flags: None affected.

    // Implementacion no seria pero necesaria para los test
    ctx->halted = true;

}

void proc_adc(cpu_context* ctx)
{
    // ADC A,r8/n8/[hl]
    // Add the value in r8 plus the carry flag to A.
    // Cycles: 1
    // Bytes: 1
    // Flags:
    // Z    Set if result is 0.
    // N    0
    // H    Set if overflow from bit 3.
    // C    Set if overflow from bit 7.

    u8 carry = getFlag(ctx, F_CARRY);
    u8 regA = readCPURegister(ctx->currentInstruction->reg_1);
    u16 suma =  regA + ctx->fetched_data + carry;
    writeCPURegister(ctx->currentInstruction->reg_1, suma);

    // Flags
    checkCarryFlag8Bits(suma, ctx);
    checkZeroFlag8Bits(suma, ctx);
    setFlag(ctx, F_N_Subtract, 0);
    checkHalfCarryFlag8Bits(regA + carry, ctx->fetched_data, ctx);
}

void proc_sbc(cpu_context* ctx)
{
    // SBC A,r8
    // SBC A,[HL]
    // SBC A,n8
    // Subtract the value in r8 and the carry flag from A.
    // Cycles: 1
    // Bytes: 1
    // Flags:
    // Z    Set if result is 0.
    // N   1
    // H   Set if borrow from bit 4.
    // C   Set if borrow (i.e. if (r8 + carry) > A).

    u8 carry = getFlag(ctx, F_CARRY);
    u8 regA = readCPURegister(ctx->currentInstruction->reg_1);
    u8 minuendo = (ctx->fetched_data + carry);
    u16 resta = regA - minuendo;
    writeCPURegister(RT_A, (u8)resta);

    //Flags
    checkZeroFlag8Bits(resta, ctx);
    setFlag(ctx, F_N_Subtract, 1);
    checkHalfBorrow8Bits(regA, (u8)(minuendo), ctx);
    checkBorrow8Bits(regA, minuendo, ctx);
}

void proc_and(cpu_context* ctx)
{
    // AND A,r8
    // AND A,[HL]
    // AND A,n8
    // Flags:
    // Z    Set if result is 0.
    // N    0
    // H    1
    // C    0

    u8 regA = readCPURegister(RT_A);
    u8 res = regA & ctx->fetched_data;
    writeCPURegister(RT_A, res);

    checkZeroFlag8Bits(res, ctx);
    setFlags(ctx, 0, 1, 0, -1);
}

void proc_xor(cpu_context* ctx)
{
    // XOR A,r8
    // XOR A,[HL]
    // XOR A,n8
    // Set A to the bitwise XOR between the value in r8 and A.
    // Cycles: 1
    // Bytes: 1
    // Flags:
    // Z    Set if result is 0.
    // N    0
    // H    0
    // C    0
    
    u8 regA = readCPURegister(RT_A);
    u8 res = regA ^ ctx->fetched_data;
    writeCPURegister(RT_A, res);

    checkZeroFlag8Bits(res, ctx);
    setFlags(ctx, 0, 0, 0, -1);
}

void proc_or(cpu_context* ctx)
{
    u8 regA = readCPURegister(RT_A);
    u8 res = regA | ctx->fetched_data;
    writeCPURegister(RT_A, res);

    checkZeroFlag8Bits(res, ctx);
    setFlags(ctx, 0, 0, 0, -1);
}

void proc_cp(cpu_context* ctx)
{
    // CP A,r8
    // CP A,[HL]
    // CP A,n8
    // ComPare the value in A with the value in r8.
    // This subtracts the value in r8 from A and sets flags accordingly, but discards the result.
    // Cycles: 1
    // Bytes: 1
    // Flags:
    // Z    Set if result is 0.
    // N    1
    // H    Set if borrow from bit 4.
    // C    Set if borrow (i.e. if r8 > A).

    u8 regA = readCPURegister(RT_A);
    u8 discarted = regA - ctx->fetched_data;

    checkBorrow8Bits(regA, ctx->fetched_data, ctx);
    checkZeroFlag8Bits(discarted, ctx);
    checkHalfBorrow8Bits(regA, ctx->fetched_data, ctx);
    setFlag(ctx, F_N_Subtract, 1);
}

void proc_pop(cpu_context* ctx)
{
    // Pop register AF from the stack. This is roughly equivalent to the following imaginary instructions:
    // LD F, [SP]  ;See below for individual flags
    // INC SP
    // LD A, [SP]
    // INC SP
    // Cycles: 3
    // Bytes: 1
    // Flags:
    // Z    Set from bit 7 of the popped low byte.
    // N    Set from bit 6 of the popped low byte.
    // H    Set from bit 5 of the popped low byte.
    // C    Set from bit 4 of the popped low byte.

    u8 low = busRead(ctx->registers.sp ++);
    u8 high = busRead(ctx->registers.sp ++);
    
    writeCPURegister(ctx->currentInstruction->reg_1, ext16bits(high, low));

    // Flags
    if(ctx->currentInstruction->reg_1 == RT_AF)
    {
        setFlags(ctx, bit4(low), bit5(low), bit6(low), bit7(low));
    }
}

void proc_push(cpu_context* ctx)
{
    // PUSH AF
    // Push register AF into the stack. This is roughly equivalent to the following imaginary instructions:
    //     DEC SP
    //     LD [SP], A
    //     DEC SP
    //     LD [SP], F.Z << 7 | F.N << 6 | F.H << 5 | F.C << 4
    // Cycles: 4
    // Bytes: 1
    // Flags: None affected.

    // PUSH r16
    // Push register r16 into the stack. This is roughly equivalent to the following imaginary instructions:
    //     DEC SP
    //     LD [SP], HIGH(r16)  ; B, D or H
    //     DEC SP
    //     LD [SP], LOW(r16)   ; C, E or L
    // Cycles: 4
    // Bytes: 1
    // Flags: None affected.

    u16 reg = readCPURegister(ctx->currentInstruction->reg_1);
    u8 high = byteAltoDeWord(reg);
    u8 low = byteBajoDeWord(reg);

    if(ctx->currentInstruction->reg_1 == RT_AF)
    { low &= 0xF0; } /*Esto porque solo interesan los flags, osea los 4 bits mas altos de F*/ 
    
    busWrite(--ctx->registers.sp, high);
    busWrite(--ctx->registers.sp, low);    

}

void proc_ret(cpu_context* ctx)
{
    // RET
    // Return from subroutine. This is basically a POP PC (if such an instruction existed). See POP r16 for an explanation of how POP works.
    // Cycles: 4
    // Bytes: 1
    // Flags: None affected.

    u8 low = busRead(ctx->registers.sp ++);
    u8 high = busRead(ctx->registers.sp ++);
    
    writeCPURegister(RT_PC, ext16bits(high, low));
}

void proc_cb(cpu_context* ctx) 
{
    // PREFIX
    // Here im gonna call the proc associate with the next instruction. Im gonna need a table of $CB instructions

    ctx->cbInst = true;

}

void proc_call(cpu_context* ctx) 
{
    // CALL n16
    // Call address n16.
    // This pushes the address of the instruction after the CALL on the stack, such that RET can pop it later; then, it executes an implicit JP n16.
    // Cycles: 6
    // Bytes: 3
    // Flags: None affected.

    // CALL cc,n16
    // Call address n16 if condition cc is met.
    // Cycles: 6 taken / 3 untaken
    // Bytes: 3
    // Flags: None affected.

    switch (ctx->currentInstruction->cond)
    {
    case CT_NONE:
        break;
    
    case CT_C:
        if (getFlag(ctx, F_CARRY) == 0) {return;}
        break;
    
    case CT_NC:
        if (getFlag(ctx, F_CARRY) == 1) {return;}
        break;

    case CT_Z:
        if (getFlag(ctx, F_ZERO) == 0) {return;}
        break;

    case CT_NZ:
        if (getFlag(ctx, F_ZERO) == 1) {return;}
        break;

    default:
        break;
    }

    // Si se cumple la condicion, entonces el return no sucede. CT_NONE es trivial

    busWrite(--ctx->registers.sp, byteAltoDeWord(ctx->registers.pc));

    busWrite(--ctx->registers.sp, byteBajoDeWord(ctx->registers.pc));
    
    ctx->registers.pc = ctx->fetched_data;
}

void proc_reti(cpu_context* ctx)
{
    // RETI
    // Return from subroutine and enable interrupts. This is basically equivalent to executing EI then RET, meaning that IME is set right after this instruction.
    // Cycles: 4
    // Bytes: 1
    // Flags: None affected.

    proc_ei(ctx);
    proc_ret(ctx);
}

void proc_ldh(cpu_context *ctx)
{
    // Copy the value in register A into the byte at address n16.
    // The destination address n16 is encoded as its 8-bit low byte and assumes a high byte of $FF, so it must be between $FF00 and $FFFF.
    // Cycles: 3
    // Bytes: 2
    // Flags: None affected.

    // LDH [n16],A
    // LDH [C],A
    // LDH A,[n16]
    // LDH A,[C]

    if (ctx->dest_is_mem)
    {
        busWrite(ctx->mem_dest, ctx->fetched_data);
    }
    else 
    {
        writeCPURegister(ctx->currentInstruction->reg_1, ctx->fetched_data);
    }

}

void proc_jphl(cpu_context *ctx)
{
    // ?

}

void proc_di(cpu_context *ctx) {ctx->int_master_enabled = false;}

void proc_ei(cpu_context *ctx) {ctx->int_master_enabled = true;}

void proc_rst(cpu_context *ctx)
{
    ctx->fetched_data = ctx->currentInstruction->param;
    proc_call(ctx);
}

void proc_err(cpu_context *ctx)
{
    //????
    printf("TO DO");
}

//CB instructions...

void proc_rlc(cpu_context *ctx)
{
    // RLC r8
    // RLC [HL]
    u8 carry;
    u8 byte;
    if (ctx->dest_is_mem)
    {
        // ┏━ Flags ━┓   ┏━━━━━━ [HL] ━━━━━┓
        // ┃    C   ←╂─┬─╂─ b7 ← ... ← b0 ←╂─┐
        // ┗━━━━━━━━━┛ │ ┗━━━━━━━━━━━━━━━━━┛ │
        //             └─────────────────────┘
        byte = ctx->fetched_data;
        carry = bit7(byte);
        byte = (byte << 1) | carry;

        busWrite(ctx->mem_dest, byte);
    }
    else
    {
        byte = readCPURegister(ctx->currentInstruction->reg_1);
        carry = bit7(byte);
        byte = (byte << 1) | carry;              
        
        writeCPURegister(ctx->currentInstruction->reg_1, byte);
    }

    //Flags
    checkZeroFlag8Bits(byte, ctx);
    setFlags(ctx, carry, 0, 0, -1);
}

void proc_rrc(cpu_context *ctx)
{
    // Rotate register r8 right.
    //   ┏━━━━━━━ r8 ━━━━━━┓   ┏━ Flags ━┓
    // ┌─╂→ b7 → ... → b0 ─╂─┬─╂→   C    ┃
    // │ ┗━━━━━━━━━━━━━━━━━┛ │ ┗━━━━━━━━━┛
    // └─────────────────────┘
    
    u8 carry;
    u8 byte;
    if (ctx->dest_is_mem)
    {
        byte = ctx->fetched_data;
        carry = byte & 0x01;
        byte = (byte >> 1) | (carry << 7);

        busWrite(ctx->mem_dest, byte);
    }
    else
    {
        byte = readCPURegister(ctx->currentInstruction->reg_1);
        carry = byte & 0x01;
        byte = (byte >> 1) | (carry << 7);              
        
        writeCPURegister(ctx->currentInstruction->reg_1, byte);
    }

    //Flags
    checkZeroFlag8Bits(byte, ctx);
    setFlags(ctx, carry, 0, 0, -1);
}

void proc_rl(cpu_context *ctx)
{
    // RL r8
    // RL [HL] 

    //Rotate register A left, through the carry flag.
    //   ┏━ Flags ━┓ ┏━━━━ r8/[HL] ━━━━┓
    // ┌─╂─   C   ←╂─╂─ b7 ← ... ← b0 ←╂─┐
    // │ ┗━━━━━━━━━┛ ┗━━━━━━━━━━━━━━━━━┛ │
    // └─────────────────────────────────┘
    
    u8 byte;
    u8 bit7;
    if (ctx->dest_is_mem)
    {
        byte = ctx->fetched_data;
        u8 bitCarry = getFlag(ctx, F_CARRY);
        bit7 = bit7(byte);
        byte = byte << 1 | bitCarry;

        busWrite(ctx->mem_dest, byte);
    }
    else
    {
        byte = readCPURegister(ctx->currentInstruction->reg_1);
        u8 bitCarry = getFlag(ctx, F_CARRY);
        bit7 = bit7(byte);
        byte = byte << 1 | bitCarry;        
        
        writeCPURegister(ctx->currentInstruction->reg_1, byte);
    }

    //Flags
    checkZeroFlag8Bits(byte, ctx);
    setFlags(ctx, bit7, 0, 0, -1);
}

void proc_rr(cpu_context *ctx)
{
    // RR r8
    // Rotate register r8 right, through the carry flag.
    //   ┏━━━━━━━ r8 ━━━━━━┓ ┏━ Flags ━┓
    // ┌─╂→ b7 → ... → b0 ─╂─╂→   C   ─╂─┐
    // │ ┗━━━━━━━━━━━━━━━━━┛ ┗━━━━━━━━━┛ │
    // └─────────────────────────────────┘
    u8 byte;
    u8 bit0;
    if (ctx->dest_is_mem)
    {
        byte = ctx->fetched_data;
        u8 bitCarry = getFlag(ctx, F_CARRY);
        bit0 = byte & 0x01;
        byte = byte >> 1 | bitCarry << 7;

        busWrite(ctx->mem_dest, byte);
    }
    else
    {
        byte = readCPURegister(ctx->currentInstruction->reg_1);
        u8 bitCarry = getFlag(ctx, F_CARRY);
        bit0 = byte & 0x01;
        byte = byte >> 1 | bitCarry << 7;        
        
        writeCPURegister(ctx->currentInstruction->reg_1, byte);
    }

    //Flags
    checkZeroFlag8Bits(byte, ctx);
    setFlags(ctx, bit0, 0, 0, -1);
}

void proc_sla(cpu_context *ctx)
{
    // Shift Left Arithmetically register r8.
    // ┏━ Flags ━┓ ┏━━━━━━━ r8 ━━━━━━┓
    // ┃    C   ←╂─╂─ b7 ← ... ← b0 ←╂─ 0
    // ┗━━━━━━━━━┛ ┗━━━━━━━━━━━━━━━━━┛

    u8 byte;
    u8 bit7;
    if (ctx->dest_is_mem)
    {
        byte = ctx->fetched_data;
        bit7 = bit7(byte);
        byte = byte << 1;

        busWrite(ctx->mem_dest, byte);
    }
    else
    {
        byte = readCPURegister(ctx->currentInstruction->reg_1);
        bit7 = bit7(byte);
        byte = byte << 1;        
        
        writeCPURegister(ctx->currentInstruction->reg_1, byte);
    }

    //Flags
    checkZeroFlag8Bits(byte, ctx);
    setFlags(ctx, bit7, 0, 0, -1);
}

void proc_sra(cpu_context *ctx)
{
    // SRA r8
    // Shift Right Arithmetically register r8 (bit 7 of r8 is unchanged).
    // ┏━━━━━━ r8 ━━━━━━┓ ┏━ Flags ━┓
    // ┃ b7 → ... → b0 ─╂─╂→   C    ┃
    // ┗━━━━━━━━━━━━━━━━┛ ┗━━━━━━━━━┛

    u8 byte;
    u8 bit0;
    if (ctx->dest_is_mem)
    {
        byte = ctx->fetched_data;
        u8 bit7 = bit7(byte);
        bit0 = byte & 0x01;
        byte = byte >> 1 | bit7 << 7;

        busWrite(ctx->mem_dest, byte);
    }
    else
    {
        byte = readCPURegister(ctx->currentInstruction->reg_1);
        u8 bit7 = bit7(byte);
        bit0 = byte & 0x01;
        byte = byte >> 1 | bit7 << 7;        
        
        writeCPURegister(ctx->currentInstruction->reg_1, byte);
    }

    //Flags
    checkZeroFlag8Bits(byte, ctx);
    setFlags(ctx, bit0, 0, 0, -1);
}

void proc_swap(cpu_context *ctx)
{
    // Swap the upper 4 bits in register r8 and the lower 4 ones.
    u8 inv;
    if (ctx->dest_is_mem)
    {
        u8 upper = (ctx->fetched_data & 0xF0) >> 4;
        u8 lower = ctx->fetched_data & 0x0F;
        inv = lower << 4 | upper;
        busWrite(ctx->mem_dest, inv);
    }
    else
    {
        u8 reg = readCPURegister(ctx->currentInstruction->reg_1);
        u8 upper = (reg & 0xF0) >> 4;
        u8 lower = reg & 0x0F;
        inv = lower << 4 | upper;
        writeCPURegister(ctx->currentInstruction->reg_1, inv);
    }
    
    // Flags
    setFlags(ctx, 0, 0, 0, inv == 0 ? 1:0);
}

void proc_srl(cpu_context *ctx)
{
    // SRL r8
    // SRL [HL]
    // Shift Right Logically register r8.
    //    ┏━━━━━━━ r8 ━━━━━━┓ ┏━ Flags ━┓
    // 0 ─╂→ b7 → ... → b0 ─╂─╂→   C    ┃
    //    ┗━━━━━━━━━━━━━━━━━┛ ┗━━━━━━━━━┛

    u8 byte;
    u8 bit0;
    if (ctx->dest_is_mem)
    {
        byte = ctx->fetched_data;
        bit0 = byte & 0x01;
        byte = byte >> 1;

        busWrite(ctx->mem_dest, byte);
    }
    else
    {
        byte = readCPURegister(ctx->currentInstruction->reg_1);
        bit0 = byte & 0x01;
        byte = byte >> 1;        
        
        writeCPURegister(ctx->currentInstruction->reg_1, byte);
    }

    //Flags
    checkZeroFlag8Bits(byte, ctx);
    setFlags(ctx, bit0, 0, 0, -1);
}

void proc_bit(cpu_context *ctx)
{
    // BIT u3,r8
    // BIT u3,[HL]
    u8 ind = ctx->currentInstruction->param;
    u8 mask = 1 << ind;
    u8 bit;
    if (ctx->dest_is_mem) // En este caso, me sirve para saber de donde cargo el byte 
    {
        bit = (ctx->fetched_data & mask) >> ind;
    }
    else
    {
        u8 reg = readCPURegister(ctx->currentInstruction->reg_1);
        bit = (reg & mask) >> ind;
    }

    // Seteo Flags
    setFlags(ctx, -1, 1, 0, bit);
}

void proc_res(cpu_context *ctx)
{
    // RES u3,r8
    // RES u3,[HL]

    u8 ind = ctx->currentInstruction->param;
    u8 mask = ~(1 << ind); // pongo un cero en u3
    u8 newByte;
    if (ctx->dest_is_mem) // En este caso, me sirve para saber de donde cargo el byte 
    {
        newByte = ctx->fetched_data & mask;
        busWrite(ctx->mem_dest, newByte);
    }
    else
    {
        u8 reg = readCPURegister(ctx->currentInstruction->reg_1);
        newByte = (reg & mask);
        writeCPURegister(ctx->currentInstruction->reg_1, newByte);
    }

    // No flags affected
}

void proc_set(cpu_context *ctx)
{
    // SET u3,r8
    // SET u3,[HL]

    u8 ind = ctx->currentInstruction->param;
    u8 mask = 1 << ind; // pongo un cero en u3
    u8 newByte;
    if (ctx->dest_is_mem) // En este caso, me sirve para saber de donde cargo el byte 
    {
        newByte = ctx->fetched_data | mask;
        busWrite(ctx->mem_dest, newByte);
    }
    else
    {
        u8 reg = readCPURegister(ctx->currentInstruction->reg_1);
        newByte = (reg | mask);
        writeCPURegister(ctx->currentInstruction->reg_1, newByte);
    }

    // No flags affected
}

in_proc processorByInstructionTypeTable[48] = 
{
    //[IN_NONE] = 0x0,
    [IN_NOP] = proc_nop,
    [IN_LD] = proc_ld,
    [IN_INC] = proc_inc,
    [IN_DEC] = proc_dec,
    [IN_RLCA] = proc_rlca,
    [IN_ADD] = proc_add,
    [IN_RRCA] = proc_rrca,
    [IN_STOP] = proc_stop,
    [IN_RLA] = proc_rla,
    [IN_JR] = proc_jr,
    [IN_RRA] = proc_rra,
    [IN_DAA] = proc_daa,
    [IN_CPL] = proc_cpl,
    [IN_SCF] = proc_scf,
    [IN_CCF] = proc_ccf,
    [IN_HALT] = proc_halt,
    [IN_ADC] = proc_adc,
    [IN_SUB] = proc_sub,
    [IN_SBC] = proc_sbc,
    [IN_AND] = proc_and,
    [IN_XOR] = proc_xor,
    [IN_OR] = proc_or,
    [IN_CP] = proc_cp,
    [IN_POP] = proc_pop,
    [IN_JP] = proc_jp,
    [IN_PUSH] = proc_push,
    [IN_RET] = proc_ret,
    [IN_CB] = proc_cb,
    [IN_CALL] = proc_call,
    [IN_RETI] = proc_reti,
    [IN_LDH] = proc_ldh,
    [IN_JPHL] = proc_jp,
    [IN_DI] = proc_di,
    [IN_EI] = proc_ei,
    [IN_RST] = proc_rst,
    [IN_ERR] = proc_err,
    //CB instructions...
    [IN_RLC] = proc_rlc, 
    [IN_RRC] = proc_rrc,
    [IN_RL] = proc_rl, 
    [IN_RR] = proc_rr,
    [IN_SLA] = proc_sla, 
    [IN_SRA] = proc_sra,
    [IN_SWAP] = proc_swap, 
    [IN_SRL] = proc_srl,
    [IN_BIT] = proc_bit, 
    [IN_RES] = proc_res,
    [IN_SET] = proc_set,
};

in_proc getProcessorForCurrentInst(cpu_context* ctx)
{
    return processorByInstructionTypeTable[ctx->currentInstruction->type];
}
