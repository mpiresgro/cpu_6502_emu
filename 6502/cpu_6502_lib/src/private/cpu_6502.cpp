#include "main_6502.h"

cpu6502::s32 cpu6502::CPU::Execute(s32 Cycles, Mem &memory)
    {
        const s32 CyclesRequested = Cycles;
        while (Cycles > 0)
        {
            Byte Instruction = Fetch_Byte(Cycles, memory);

            switch (Instruction)
            {

            case INS_LDA_IM:
            {
                Byte Value = Fetch_Byte(Cycles, memory);
                A = Value;
                LDA_SET_STATUS();
            }
            break;

            case INS_LDA_ZEROP:
            {
                Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
                A = ReadByte(Cycles, memory, ZeroPageAddress);
                LDA_SET_STATUS();
            }
            break;

            case INS_LDA_ZEROP_X:
            {
                Byte ZeroPageAddress = Fetch_Byte(Cycles, memory);
                ZeroPageAddress += X;
                Cycles--;
                A = ReadByte(Cycles, memory, ZeroPageAddress);
                LDA_SET_STATUS();
            }
            break;

            case INS_LDA_ABS:
            {
                Word AbsoluteAdress = Fetch_Word(Cycles, memory);
                A = ReadByte(Cycles, memory, AbsoluteAdress);
                LDA_SET_STATUS();
            }
            break;

            case INS_LDA_ABS_X:
            {
                Word AbsoluteAdress = Fetch_Word(Cycles, memory);
                Word AbsoluteAdress_X = AbsoluteAdress + X;
                A = ReadByte(Cycles, memory, AbsoluteAdress_X);
                bool CrossingPage = (AbsoluteAdress % 256) + X > 0xFE; 
                if (CrossingPage)
                {
                    Cycles--;
                }
                LDA_SET_STATUS();
            }
            break;

            case INS_LDA_ABS_Y:
            {
                Word AbsoluteAdress = Fetch_Word(Cycles, memory);
                Word AbsoluteAdress_Y = AbsoluteAdress + Y;
                A = ReadByte(Cycles, memory, AbsoluteAdress_Y);
                bool CrossingPage = (AbsoluteAdress % 256) + Y > 0xFE; 
                if (CrossingPage)
                {
                    Cycles--;
                }
                LDA_SET_STATUS();
            }
            break;

            case INS_LDA_IND_X:
            {
                Byte ZAddress = Fetch_Byte(Cycles, memory);
                ZAddress += X;
                Cycles--;
                Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
                A = ReadByte(Cycles, memory, EffectiveAddress);
                LDA_SET_STATUS();
            }
            break;

            case INS_LDA_IND_Y:
            {
                Byte ZAddress = Fetch_Byte(Cycles, memory);
                Word EffectiveAddress = ReadWord(Cycles, memory, ZAddress);
                Word EffectiveAddress_Y  = EffectiveAddress +  Y;
                A = ReadByte(Cycles, memory, EffectiveAddress_Y);
                bool CrossingPage = (EffectiveAddress % 256) + Y > 0xFE; 
                if (CrossingPage)
                {
                    Cycles--;
                }
                LDA_SET_STATUS();
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