#include "main_6502.h"

cpu6502::s32 cpu6502::CPU::Execute(s32 Cycles, Mem &memory)
{
    // Lambda function to load A, X, Y Register with a given Adress
    auto LoadRegister = [&Cycles, &memory, this](Byte &Register, Word Address) {
        Register = ReadByte(Cycles, memory, Address);
        Set_Zero_and_Negative_Flags(Register);
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
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            LoadRegister(A, AbsoluteAdress);
        }
        break;

        case INS_LDX_ABS:
        {
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            LoadRegister(X, AbsoluteAdress);
        }
        break;

        case INS_LDY_ABS:
        {
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            LoadRegister(Y, AbsoluteAdress);
        }
        break;

        // Load Register - Absolute X
        case INS_LDA_ABS_X:
        {
            Word AbsoluteAdress_X = AbsoluteWithOffset(Cycles, memory, X);
            LoadRegister(A, AbsoluteAdress_X);
        }
        break;

        case INS_LDY_ABS_X:
        {
            Word AbsoluteAdress_X = AbsoluteWithOffset(Cycles, memory, X);
            LoadRegister(Y, AbsoluteAdress_X);
        }
        break;

        // Load Register - Absolute Y
        case INS_LDA_ABS_Y:
        {
            Word AbsoluteAdress_Y = AbsoluteWithOffset(Cycles, memory, Y);
            LoadRegister(A, AbsoluteAdress_Y);
        }
        break;

        case INS_LDX_ABS_Y:
        {
            Word AbsoluteAdress_Y = AbsoluteWithOffset(Cycles, memory, Y);
            LoadRegister(X, AbsoluteAdress_Y);
        }
        break;

        case INS_LDA_IND_X:
        {
            // TODO: Check if INS_LDA_IND_X needs CrossingPage check
            Byte ZAddress = Fetch_Byte(Cycles, memory);
            ZAddress += X;
            Cycles--;
            Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
            LoadRegister(A, EffectiveAddress);
        }
        break;

        case INS_LDA_IND_Y:
        {
            Byte ZAddress = Fetch_Byte(Cycles, memory);
            Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
            Word EffectiveAddress_Y = EffectiveAddress + Y;
            bool CrossingPage = (EffectiveAddress % 256) + Y > 0xFE;
            if (CrossingPage)
            {
                Cycles--;
            }
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
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            WriteByte(A, AbsoluteAdress, Cycles, memory);
        }
        break;

        case INS_STX_ABS:
        {
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            WriteByte(X, AbsoluteAdress, Cycles, memory);
        }
        break;

        case INS_STY_ABS:
        {
            Word AbsoluteAdress = Fetch_Word(Cycles, memory);
            WriteByte(Y, AbsoluteAdress, Cycles, memory);
        }
        break;

        case INS_STA_ABS_X:
        {
            Word AbsoluteAdress_X = AbsoluteWithOffset(Cycles, memory, X);
            WriteByte(A, AbsoluteAdress_X, Cycles, memory);
        }
        break;

        case INS_STA_ABS_Y:
        {
            Word AbsoluteAdress_Y = AbsoluteWithOffset(Cycles, memory, Y);
            WriteByte(A, AbsoluteAdress_Y, Cycles, memory);
        }
        break;

        case INS_STA_IND_X:
        {
            Byte ZAddress = Fetch_Byte(Cycles, memory);
            ZAddress += X;
            Cycles--;
            Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
            WriteByte(A, EffectiveAddress, Cycles, memory);
        }
        break;

        case INS_STA_IND_Y:
        {
            Byte ZAddress = Fetch_Byte(Cycles, memory);
            Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
            Word EffectiveAddress_Y = EffectiveAddress + Y;
            Cycles--;
            WriteByte(A, EffectiveAddress_Y, Cycles, memory);
        }
        break;

        case INS_JSR:
        {
            Word SubroutineAddr = Fetch_Word(Cycles, memory);
            WriteWord(PC - 1, SP, Cycles, memory);
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