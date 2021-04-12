#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// http://www.obelisk.me.uk/6502/
// https://github.com/davepoo/6502Emulator/blob/master/6502/6502Lib

namespace cpu6502
{
    using Byte = unsigned char;  // 8 bits
    using Word = unsigned short; // 16 bits

    using u32 = unsigned int;
    using s32 = signed int;
    struct Mem;
    struct CPU;
    struct ProcessorFlags;
}

struct cpu6502::Mem
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
};

struct cpu6502::ProcessorFlags
{

    // Status Flags - C++ bit field
    Byte C : 1;
    Byte Z : 1;
    Byte I : 1;
    Byte D : 1;
    Byte B : 1;
    Byte NotUsedBit : 1;
    Byte V : 1;
    Byte N : 1;
};

struct cpu6502::CPU
{

    Word PC; // Program Counter
    Byte SP; // Stack Pointer

    // Registers
    Byte A; // Accumulator
    Byte X, Y;

    union
    {
        Byte PS;
        ProcessorFlags flags;
    };

    void Reset(Mem &memory, Word ResetVector = 0)
    {
        // Use 0xFFFC as default reset vector
        PC = (ResetVector) ? ResetVector : 0xFFFC;
        SP = 0xFF; // system stack ($0100-$01FF)
        A = X = Y = 0;
        flags.C = flags.Z = flags.I = flags.D = flags.B = flags.V = flags.N = 0;
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

    void WriteByte(Byte Value, u32 Address, s32 &Cycles, Mem &memory)
    {
        memory[Address] = Value;
        Cycles--;
    }

    Word ReadWord(s32 &Cycles, Mem &memory, Word Address)
    {
        Byte LowByte = ReadByte(Cycles, memory, Address);
        Byte HighByte = ReadByte(Cycles, memory, Address + 1);
        return (HighByte << 8) | LowByte;
    }

    void WriteWord(Word Value, u32 Address, s32 &Cycles, Mem &memory)
    {
        // Write two bytes
        memory[Address] = Value & 0xFF;     // get first byte
        memory[Address + 1] = (Value >> 8); // get second byte
        Cycles -= 2;                        // Cycles pass by value;
    }

    Word SPTo16Address() const
    {
        // returns Stack Pointer as a 16bits address
        return 0x100 | SP;
    }

    void PushWordToStack(s32 &Cycles, Mem &memory, Word Value)
    {
        // MSB
        WriteByte(Value >> 8, SPTo16Address(), Cycles, memory);
        SP--;
        // LSB
        WriteByte(Value & 0xFF, SPTo16Address(), Cycles, memory);
        SP--;
    }

    void PushPCMinusOneToStack(s32 &Cycles, Mem &memory)
    {
        // Push the program counter into the Stack
        PushWordToStack(Cycles, memory, PC - 1);
    }

    Word PopWordFromStack(s32 &Cycles, Mem &memory)
    {
        Word Value = ReadWord(Cycles, memory, SPTo16Address() + 1);
        SP += 2;
        Cycles--;
        return Value;
    }

    void Set_Zero_and_Negative_Flags(Byte Register)
    {
        flags.Z = (Register == 0);
        flags.N = (Register & 0b10000000) > 0;
    }

    // Op Codes
    static constexpr Byte
        // Load Register
        INS_LDA_IM = 0xA9,      // Load Accumulator Immediate Mode
        INS_LDA_ZEROP = 0xA5,   // Load Accumulator Zero Page Mode
        INS_LDA_ZEROP_X = 0xB5, // Load Accumulator Zero Page X Mode
        INS_LDA_ABS = 0xAD,     // Load Accumulator Absolute Mode
        INS_LDA_ABS_X = 0xBD,   // Load Accumulator Absolute X Mode
        INS_LDA_ABS_Y = 0xB9,   // Load Accumulator Absolute Y Mode
        INS_LDA_IND_X = 0xA1,   // Load Accumulator Inderect X Mode
        INS_LDA_IND_Y = 0xB1,   // Load Accumulator Inderect Y Mode

        INS_LDX_IM = 0xA2,      // Load X Immediate Mode
        INS_LDX_ZEROP = 0xA6,   // Load X Zero Page Mode
        INS_LDX_ZEROP_Y = 0xB6, // Load X Zero Page Y Mode
        INS_LDX_ABS = 0xAE,     // Load X Absolute Mode
        INS_LDX_ABS_Y = 0xBE,   // Load X Absolute Y Mode

        INS_LDY_IM = 0xA0,      // Load Y Immediate Mode
        INS_LDY_ZEROP = 0xA4,   // Load Y Zero Page Mode
        INS_LDY_ZEROP_X = 0xB4, // Load Y Zero Page X Mode
        INS_LDY_ABS = 0xAC,     // Load Y Absolute Mode
        INS_LDY_ABS_X = 0xBC,   // Load Y Absolute X Mode

        // Store Register
        INS_STA_ZEROP = 0x85,   // Store Accumulator Zero Page Mode
        INS_STA_ZEROP_X = 0x95, // Store Accumulator Zero Page X Mode
        INS_STA_ABS = 0x8D,     // Store Accumulator Absolute Mode
        INS_STA_ABS_X = 0x9D,   // Store Accumulator Absolute X Mode
        INS_STA_ABS_Y = 0x99,   // Store Accumulator Absolute Y Mode
        INS_STA_IND_X = 0x81,   // Store Accumulator Inderect X Mode
        INS_STA_IND_Y = 0x91,   // Store Accumulator Inderect Y Mode

        INS_STX_ZEROP = 0x86,   // Store X Zero Page Mode
        INS_STX_ZEROP_Y = 0x96, // Store X Zero Page Y Mode
        INS_STX_ABS = 0x8E,     // Store X Absolute Mode

        INS_STY_ZEROP = 0x84,   // Store Y Zero Page Mode
        INS_STY_ZEROP_X = 0x94, // Store Y Zero Page X Mode
        INS_STY_ABS = 0x8C,     // Store Y Absolute Mode

        INS_JSR = 0x20,     // Jump to Subroutine
        INS_RTS = 0x60,     // Return from Subroutine
        INS_JMP_ABS = 0x4C, // Jump Absolute
        INS_JMP_IND = 0x6C; // Jump Indirect

    s32 Execute(s32 Cycles, Mem &memory);

    Byte ZeroPageWithOffset(s32 &Cycles, Mem &memory, Byte &OffSet);

    Word AbsoluteWithOffset(s32 &Cycles, Mem &memory, Byte &OffSet);
};
