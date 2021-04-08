#include "main_6502.h"

cpu6502::s32 cpu6502::CPU::Execute(s32 Cycles, Mem &memory)
{
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
            A = ReadByte(Cycles, memory, ZeroPageAddress);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_LDX_ZEROP:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            X = ReadByte(Cycles, memory, ZeroPageAddress);
            Set_Zero_and_Negative_Flags(X);
        }
        break;

        case INS_LDY_ZEROP:
        {
            Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
            Y = ReadByte(Cycles, memory, ZeroPageAddress);
            Set_Zero_and_Negative_Flags(Y);
        }
        break;

        // Load Register - Zero Page X Offset
        case INS_LDA_ZEROP_X:
        {
            Byte ZeroPageXAddress = ZeroPageWithOffset(Cycles, memory, X);
            A = ReadByte(Cycles, memory, ZeroPageXAddress);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_LDY_ZEROP_X:
        {
            Byte ZeroPageXAddress = ZeroPageWithOffset(Cycles, memory, X);
            Y = ReadByte(Cycles, memory, ZeroPageXAddress);
            Set_Zero_and_Negative_Flags(Y);
        }
        break;

        // Load Register - Zero Page Y Offset
        case INS_LDX_ZEROP_Y:
        {
            Byte ZeroPageYAddress = ZeroPageWithOffset(Cycles, memory, Y);
            X = ReadByte(Cycles, memory, ZeroPageYAddress);
            Set_Zero_and_Negative_Flags(X);
        }
        break;

        // Load Register - Absolute
        case INS_LDA_ABS:
        {
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            A = ReadByte(Cycles, memory, AbsoluteAdress);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_LDX_ABS:
        {
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            X = ReadByte(Cycles, memory, AbsoluteAdress);
            Set_Zero_and_Negative_Flags(X);
        }
        break;

        case INS_LDY_ABS:
        {
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            Y = ReadByte(Cycles, memory, AbsoluteAdress);
            Set_Zero_and_Negative_Flags(Y);
        }
        break;

        // Load Register - Absolute X
        case INS_LDA_ABS_X:
        {
            Word AbsoluteAdress_X = AbsoluteWithOffset(Cycles, memory, X);
            A = ReadByte(Cycles, memory, AbsoluteAdress_X);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_LDY_ABS_X:
        {
            Word AbsoluteAdress_X = AbsoluteWithOffset(Cycles, memory, X);
            Y = ReadByte(Cycles, memory, AbsoluteAdress_X);
            Set_Zero_and_Negative_Flags(Y);
        }
        break;

        // Load Register - Absolute Y
        case INS_LDA_ABS_Y:
        {
            Word AbsoluteAdress_Y = AbsoluteWithOffset(Cycles, memory, Y);
            A = ReadByte(Cycles, memory, AbsoluteAdress_Y);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_LDX_ABS_Y:
        {
            Word AbsoluteAdress_Y = AbsoluteWithOffset(Cycles, memory, Y);
            X = ReadByte(Cycles, memory, AbsoluteAdress_Y);
            Set_Zero_and_Negative_Flags(X);
        }
        break;

        case INS_LDA_IND_X:
        {
            Byte ZAddress = Fetch_Byte(Cycles, memory);
            ZAddress += X;
            Cycles--;
            Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
            A = ReadByte(Cycles, memory, EffectiveAddress);
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_LDA_IND_Y:
        {
            Byte ZAddress = Fetch_Byte(Cycles, memory);
            Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
            Word EffectiveAddress_Y = EffectiveAddress + Y;
            A = ReadByte(Cycles, memory, EffectiveAddress_Y);
            bool CrossingPage = (EffectiveAddress % 256) + Y > 0xFE;
            if (CrossingPage)
            {
                Cycles--;
            }
            Set_Zero_and_Negative_Flags(A);
        }
        break;

        case INS_JSR:
        {
            Word SubroutineAddr = Fetch_Word(Cycles, memory);
            memory.WriteWord(PC - 1, SP, Cycles);
            SP += 2;
            PC = SubroutineAddr;
            Cycles--;
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

    Word AbsoluteAdress = Fetch_Word(Cycles, memory);
    Word AbsoluteAdress_Offset = AbsoluteAdress + OffSet;
    bool CrossingPage = (AbsoluteAdress % 256) + OffSet > 0xFE;
    if (CrossingPage)
    {
        Cycles--;
    }
    return AbsoluteAdress_Offset;
}