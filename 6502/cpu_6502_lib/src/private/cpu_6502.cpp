#include "main_6502.h"

cpu6502::s32 cpu6502::CPU::Execute(s32 Cycles, Mem &memory)
{
    // Lambda function to load A, X, Y Register with a given Address
    auto LoadRegister = [&Cycles, &memory, this](Byte &Register, Word Address) {
        Register = ReadByte(Cycles, memory, Address);
        Set_Zero_and_Negative_Flags(Register);
    };

    auto And = [&Cycles, &memory, this](Word Address) {
        A &= ReadByte(Cycles, memory, Address);
        Set_Zero_and_Negative_Flags(A);
    };

    auto Eor = [&Cycles, &memory, this](Word Address) {
        A ^= ReadByte(Cycles, memory, Address);
        Set_Zero_and_Negative_Flags(A);
    };

    auto Ora = [&Cycles, &memory, this](Word Address) {
        A |= ReadByte(Cycles, memory, Address);
        Set_Zero_and_Negative_Flags(A);
    };

    auto MemOp = [&Cycles, &memory, this](Word Address, char Operation) {
        Byte Value = ReadByte(Cycles, memory, Address);
        if (Operation == 'I')
            Value++;
        else if (Operation == 'D')
            Value--;
        else
            throw -1;

        Cycles--;
        WriteByte(Value, Address, Cycles, memory);
        Set_Zero_and_Negative_Flags(Value);
    };

    const s32 CyclesRequested = Cycles;
    while (Cycles > 0)
    {
        Byte Instruction = Fetch_Byte(Cycles, memory);

        switch (Instruction)
        {

        // Load Register - Immediate
        case INS_LDA_IM:
        {
            A = Fetch_Byte(Cycles, memory);
            Set_Zero_and_Negative_Flags(A);
        }
        break;
        case INS_LDX_IM:
        {
            X = Fetch_Byte(Cycles, memory);
            Set_Zero_and_Negative_Flags(X);
        }
        break;
        case INS_LDY_IM:
        {
            Y = Fetch_Byte(Cycles, memory);
            Set_Zero_and_Negative_Flags(Y);
        }
        break;

        // Load Register - Zero Page
        case INS_LDA_ZEROP:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            LoadRegister(A, ZeroPageAddress);
        }
        break;

        case INS_LDX_ZEROP:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            LoadRegister(X, ZeroPageAddress);
        }
        break;

        case INS_LDY_ZEROP:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            LoadRegister(Y, ZeroPageAddress);
        }
        break;

        // Load Register - Zero Page X Offset
        case INS_LDA_ZEROP_X:
        {
            Byte ZeroPageXAddress = ZeroPageWithOffset(Cycles, memory, X);
            LoadRegister(A, ZeroPageXAddress);
        }
        break;

        case INS_LDY_ZEROP_X:
        {
            Byte ZeroPageXAddress = ZeroPageWithOffset(Cycles, memory, X);
            LoadRegister(Y, ZeroPageXAddress);
        }
        break;

        // Load Register - Zero Page Y Offset
        case INS_LDX_ZEROP_Y:
        {
            Byte ZeroPageYAddress = ZeroPageWithOffset(Cycles, memory, Y);
            LoadRegister(X, ZeroPageYAddress);
        }
        break;

        // Load Register - Absolute
        case INS_LDA_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            LoadRegister(A, AbsoluteAddress);
        }
        break;

        case INS_LDX_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            LoadRegister(X, AbsoluteAddress);
        }
        break;

        case INS_LDY_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            LoadRegister(Y, AbsoluteAddress);
        }
        break;

        // Load Register - Absolute X
        case INS_LDA_ABS_X:
        {
            Word AbsoluteAddress_X = AbsoluteWithOffset(Cycles, memory, X);
            LoadRegister(A, AbsoluteAddress_X);
        }
        break;

        case INS_LDY_ABS_X:
        {
            Word AbsoluteAddress_X = AbsoluteWithOffset(Cycles, memory, X);
            LoadRegister(Y, AbsoluteAddress_X);
        }
        break;

        // Load Register - Absolute Y
        case INS_LDA_ABS_Y:
        {
            Word AbsoluteAddress_Y = AbsoluteWithOffset(Cycles, memory, Y);
            LoadRegister(A, AbsoluteAddress_Y);
        }
        break;

        case INS_LDX_ABS_Y:
        {
            Word AbsoluteAddress_Y = AbsoluteWithOffset(Cycles, memory, Y);
            LoadRegister(X, AbsoluteAddress_Y);
        }
        break;

        case INS_LDA_IND_X:
        {
            Word EffectiveAddress = IndirectX(Cycles, memory);
            LoadRegister(A, EffectiveAddress);
        }
        break;

        case INS_LDA_IND_Y:
        {
            Word EffectiveAddress_Y = IndirectY(Cycles, memory);
            LoadRegister(A, EffectiveAddress_Y);
        }
        break;

        case INS_STA_ZEROP:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            WriteByte(A, ZeroPageAddress, Cycles, memory);
        }
        break;

        case INS_STX_ZEROP:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            WriteByte(X, ZeroPageAddress, Cycles, memory);
        }
        break;

        case INS_STY_ZEROP:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            WriteByte(Y, ZeroPageAddress, Cycles, memory);
        }
        break;

        case INS_STA_ZEROP_X:
        {
            Byte ZeroPageXAddress = ZeroPageWithOffset(Cycles, memory, X);
            WriteByte(A, ZeroPageXAddress, Cycles, memory);
        }
        break;

        case INS_STY_ZEROP_X:
        {
            Byte ZeroPageXAddress = ZeroPageWithOffset(Cycles, memory, X);
            WriteByte(Y, ZeroPageXAddress, Cycles, memory);
        }
        break;

        case INS_STX_ZEROP_Y:
        {
            Byte ZeroPageYAddress = ZeroPageWithOffset(Cycles, memory, Y);
            WriteByte(Y, ZeroPageYAddress, Cycles, memory);
        }
        break;

        case INS_STA_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            WriteByte(A, AbsoluteAddress, Cycles, memory);
        }
        break;

        case INS_STX_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            WriteByte(X, AbsoluteAddress, Cycles, memory);
        }
        break;

        case INS_STY_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            WriteByte(Y, AbsoluteAddress, Cycles, memory);
        }
        break;

        case INS_STA_ABS_X:
        {
            Word AbsoluteAddress_X = AbsoluteWithOffset(Cycles, memory, X);
            WriteByte(A, AbsoluteAddress_X, Cycles, memory);
        }
        break;

        case INS_STA_ABS_Y:
        {
            Word AbsoluteAddress_Y = AbsoluteWithOffset(Cycles, memory, Y);
            WriteByte(A, AbsoluteAddress_Y, Cycles, memory);
        }
        break;

        case INS_STA_IND_X:
        {
            Word EffectiveAddress = IndirectX(Cycles, memory);
            WriteByte(A, EffectiveAddress, Cycles, memory);
        }
        break;

        case INS_STA_IND_Y:
        {
            // Uses 6 Cycles - No CrossingPageCheck
            Word EffectiveAddress_Y = IndirectY_6(Cycles, memory);
            WriteByte(A, EffectiveAddress_Y, Cycles, memory);
        }
        break;

        case INS_JSR:
        {
            Word SubroutineAddr = Fetch_Word(Cycles, memory);
            // Save PC in Stack
            PushPCMinusOneToStack(Cycles, memory);
            // Change PC to Jump Address
            PC = SubroutineAddr;
            Cycles--;
        }
        break;

        case INS_RTS:
        {
            // Get PC From Stack
            Word ReturnAddress = PopWordFromStack(Cycles, memory);
            PC = ReturnAddress + 1;
            Cycles -= 2;
        }
        break;
        case INS_JMP_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            PC = AbsoluteAddress;
        }
        break;
        case INS_JMP_IND:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            Word JumpAddress = ReadWord(Cycles, memory, AbsoluteAddress);
            PC = JumpAddress;
        }
        break;

        case INS_TSX:
        {
            X = SP;
            Cycles--;
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_TXS:
        {
            SP = X;
            Cycles--;
        }
        break;

        case INS_PHA:
        {
            PushByteToStack(Cycles, memory, A);
            Cycles--;
        }
        break;

        case INS_PHP:
        {
            PushByteToStack(Cycles, memory, PS);
            Cycles--;
        }
        break;

        case INS_PLA:
        {
            A = PopByteFromStack(Cycles, memory);
            Cycles--;
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_PLP:
        {
            PS = PopByteFromStack(Cycles, memory);
            Cycles--;
        }
        break;

        case INS_AND_IM:
        {
            A &= Fetch_Byte(Cycles, memory);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_AND_ZERO_P:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            And(ZeroPageAddress);
        }
        break;

        case INS_AND_ZERO_PX:
        {
            Byte ZeroPageXOffsetAddress = ZeroPageWithOffset(Cycles, memory, X);
            And(ZeroPageXOffsetAddress);
        }
        break;

        case INS_AND_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            And(AbsoluteAddress);
        }
        break;

        case INS_AND_ABS_X:
        {
            Word AbsoluteXAddress = AbsoluteWithOffset(Cycles, memory, X);
            And(AbsoluteXAddress);
        }
        break;

        case INS_AND_ABS_Y:
        {
            Word AbsoluteYAddress = AbsoluteWithOffset(Cycles, memory, Y);
            And(AbsoluteYAddress);
        }
        break;

        case INS_AND_IND_X:
        {
            Word EffectiveAddress = IndirectX(Cycles, memory);
            And(EffectiveAddress);
        }
        break;

        case INS_AND_IND_Y:
        {
            Word EffectiveAddress = IndirectY(Cycles, memory);
            And(EffectiveAddress);
        }
        break;

        case INS_EOR_IM:
        {
            A ^= Fetch_Byte(Cycles, memory);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_EOR_ZERO_P:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            Eor(ZeroPageAddress);
        }
        break;

        case INS_EOR_ZERO_PX:
        {
            Byte ZeroPageXOffsetAddress = ZeroPageWithOffset(Cycles, memory, X);
            Eor(ZeroPageXOffsetAddress);
        }
        break;

        case INS_EOR_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            Eor(AbsoluteAddress);
        }
        break;

        case INS_EOR_ABS_X:
        {
            Word AbsoluteXAddress = AbsoluteWithOffset(Cycles, memory, X);
            Eor(AbsoluteXAddress);
        }
        break;

        case INS_EOR_ABS_Y:
        {
            Word AbsoluteYAddress = AbsoluteWithOffset(Cycles, memory, Y);
            Eor(AbsoluteYAddress);
        }
        break;

        case INS_EOR_IND_X:
        {
            Word EffectiveAddress = IndirectX(Cycles, memory);
            Eor(EffectiveAddress);
        }
        break;

        case INS_EOR_IND_Y:
        {
            Word EffectiveAddress = IndirectY(Cycles, memory);
            Eor(EffectiveAddress);
        }
        break;

        case INS_ORA_IM:
        {
            A |= Fetch_Byte(Cycles, memory);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_ORA_ZERO_P:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            Ora(ZeroPageAddress);
        }
        break;

        case INS_ORA_ZERO_PX:
        {
            Byte ZeroPageXOffsetAddress = ZeroPageWithOffset(Cycles, memory, X);
            Ora(ZeroPageXOffsetAddress);
        }
        break;

        case INS_ORA_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            Ora(AbsoluteAddress);
        }
        break;

        case INS_ORA_ABS_X:
        {
            Word AbsoluteXAddress = AbsoluteWithOffset(Cycles, memory, X);
            Ora(AbsoluteXAddress);
        }
        break;

        case INS_ORA_ABS_Y:
        {
            Word AbsoluteYAddress = AbsoluteWithOffset(Cycles, memory, Y);
            Ora(AbsoluteYAddress);
        }
        break;

        case INS_ORA_IND_X:
        {
            Word EffectiveAddress = IndirectX(Cycles, memory);
            Ora(EffectiveAddress);
        }
        break;

        case INS_ORA_IND_Y:
        {
            Word EffectiveAddress = IndirectY(Cycles, memory);
            Ora(EffectiveAddress);
        }
        break;

        case INS_BIT_ZERO_P:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            Byte Value = A & ReadByte(Cycles, memory, ZeroPageAddress);
            Set_BIT_Flags(Value);
        }
        break;

        case INS_BIT_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            Byte Value = A & ReadByte(Cycles, memory, AbsoluteAddress);
            Set_BIT_Flags(Value);
        }
        break;

        case INS_TAX:
        {
            X = A;
            Cycles--;
            Set_Zero_and_Negative_Flags(X);
        }
        break;

        case INS_TAY:
        {
            Y = A;
            Cycles--;
            Set_Zero_and_Negative_Flags(Y);
        }
        break;

        case INS_TXA:
        {
            A = X;
            Cycles--;
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_TYA:
        {
            A = Y;
            Cycles--;
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_INC_ZERO_P:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            MemOp(ZeroPageAddress, 'I');
        }
        break;

        case INS_INC_ZERO_PX:
        {
            Byte ZeroPageXOffsetAddress = ZeroPageWithOffset(Cycles, memory, X);
            MemOp(ZeroPageXOffsetAddress, 'I');
        }
        break;

        case INS_INC_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            MemOp(AbsoluteAddress, 'I');
        }
        break;

        case INS_INC_ABS_X:
        {
            Word AbsoluteXAddress = AbsoluteWithOffset_5(Cycles, memory, X);
            MemOp(AbsoluteXAddress, 'I');
        }
        break;

        case INS_DEC_ZERO_P:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            MemOp(ZeroPageAddress, 'D');
        }
        break;

        case INS_DEC_ZERO_PX:
        {
            Byte ZeroPageXOffsetAddress = ZeroPageWithOffset(Cycles, memory, X);
            MemOp(ZeroPageXOffsetAddress, 'D');
        }
        break;

        case INS_DEC_ABS:
        {
            Word AbsoluteAddress = Fetch_Word(Cycles, memory);
            MemOp(AbsoluteAddress, 'D');
        }
        break;

        case INS_DEC_ABS_X:
        {
            Word AbsoluteXAddress = AbsoluteWithOffset_5(Cycles, memory, X);
            MemOp(AbsoluteXAddress, 'D');
        }
        break;

        default:
            printf("\nInstruction %d not handled\n", Instruction);
            throw -1;
            break;
        }
    }
    // Cycles should be 0 at this point
    return CyclesRequested - Cycles;
}

cpu6502::Byte cpu6502::CPU::ZeroPageWithOffset(s32 &Cycles, Mem &memory, Byte &OffSet)
{

    Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
    ZeroPageAddress += OffSet;
    Cycles--;
    return ZeroPageAddress;
}

cpu6502::Word cpu6502::CPU::AbsoluteWithOffset(s32 &Cycles, Mem &memory, Byte &OffSet)
{

    Word AbsoluteAddress = Fetch_Word(Cycles, memory);
    Word AbsoluteAddress_Offset = AbsoluteAddress + OffSet;
    bool CrossingPage = (AbsoluteAddress % 256) + OffSet > 0xFE;
    if (CrossingPage)
    {
        Cycles--;
    }
    return AbsoluteAddress_Offset;
}

cpu6502::Word cpu6502::CPU::AbsoluteWithOffset_5(s32 &Cycles, Mem &memory, Byte &OffSet)
{

    Word AbsoluteAddress = Fetch_Word(Cycles, memory);
    Word AbsoluteAddress_Offset = AbsoluteAddress + OffSet;
    Cycles--;
    return AbsoluteAddress_Offset;
}

cpu6502::Word cpu6502::CPU::IndirectX(s32 &Cycles, Mem &memory)
{
    Byte ZAddress = Fetch_Byte(Cycles, memory);
    ZAddress += X;
    Cycles--;
    return ReadWord(Cycles, memory, ZAddress);
}

cpu6502::Word cpu6502::CPU::IndirectY(s32 &Cycles, Mem &memory)
{
    Byte ZAddress = Fetch_Byte(Cycles, memory);
    Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
    Word EffectiveAddress_Y = EffectiveAddress + Y;
    bool CrossingPage = (EffectiveAddress % 256) + Y > 0xFE;
    if (CrossingPage)
    {
        Cycles--;
    }
    return EffectiveAddress_Y;
}

cpu6502::Word cpu6502::CPU::IndirectY_6(s32 &Cycles, Mem &memory)
{
    Byte ZAddress = Fetch_Byte(Cycles, memory);
    Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
    Cycles--;
    return EffectiveAddress + Y;
}