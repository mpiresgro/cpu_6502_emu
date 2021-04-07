#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// http://www.obelisk.me.uk/6502/
// https://github.com/davepoo/6502Emulator/blob/master/6502/6502Lib

using Byte = unsigned char;  // 8 bits
using Word = unsigned short; // 16 bits

using u32 = unsigned int;
using s32 = signed int;

struct Mem
{

    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void Init()
    {
        //  cleans Data array;
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0;
        }
    }

    Byte operator[](u32 Address) const
    {
        // Read one byte
        assert(Address < MAX_MEM);
        return Data[Address];
    }

    Byte &operator[](u32 Address)
    {
        // Write one byte - returns memory address of Data[Address]
        assert(Address < MAX_MEM);
        return Data[Address];
    }

    void WriteWord(Word Value, u32 Address, s32 &Cycles)
    {
        // Write two bytes
        assert(Address + 1 < MAX_MEM);
        Data[Address] = Value & 0xFF;     // get first byte
        Data[Address + 1] = (Value >> 8); // get second byte
        Cycles -= 2;                      // Cycles pass by value;
    }
};

struct CPU
{

    Word PC; // Program Counter
    Word SP; // Stack Pointer (!! Should be Byte)

    // Registers
    Byte A; // Accumulator
    Byte X, Y;

    // Status Flags - C++ bit field
    Byte C : 1;
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte V : 1;
    Byte N : 1;

    void Reset(Mem &memory)
    {
        PC = 0xFFFC;
        SP = 0x0100;
        A = X = Y = 0;
        C = Z = I = D = B = V = N = 0;
        memory.Init();
    }

    Byte Fetch_Byte(s32 &Cycles, Mem &memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Word Fetch_Word(s32 &Cycles, Mem &memory)
    {
        // cpu 6502 -> little endian
        Word Data = memory[PC];
        PC++;
        // | -> or operator
        Data |= (memory[PC] << 8);
        PC++;

        Cycles -= 2;
        return Data;
    }

    Byte ReadByte(s32 &Cycles, Mem &memory, Word Address)
    {
        Byte Data = memory[Address];
        Cycles--;
        return Data;
    }

    Word ReadWord(s32 &Cycles, Mem &memory, Word Address)
    {
        Byte LowByte = ReadByte(Cycles, memory, Address);
        Byte HighByte = ReadByte(Cycles, memory, Address + 1);
        return (HighByte << 8) | LowByte;
    }

    void LDA_SET_STATUS()
    {
        Z = (A == 0);
        N = (A & 0b10000000) > 0;
    }

    // Op Codes
    static constexpr Byte
        INS_LDA_IM = 0xA9,      // Load Accumulator Immediate Mode
        INS_LDA_ZEROP = 0xA5,   // Load Accumulator Zero Page Mode
        INS_LDA_ZEROP_X = 0xB5, // Load Accumulator Zero Page X Mode
        INS_LDA_ABS = 0xAD,     // Load Accumulator Absolute Mode
        INS_LDA_ABS_X = 0xBD,   // Load Accumulator Absolute X Mode
        INS_LDA_ABS_Y = 0xB9,   // Load Accumulator Absolute Y Mode
        INS_LDA_IND_X = 0xA1,   // Load Accumulator Inderect X Mode
        INS_LDA_IND_Y = 0xB1,   // Load Accumulator Inderect Y Mode
        INS_JSR = 0x20;         // Jump to Subroutine

    s32 Execute(s32 Cycles, Mem &memory)
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
};
