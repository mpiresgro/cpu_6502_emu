#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502IncrementsAndDecrementsTests : public testing::Test
{
public:
    cpu6502::Mem mem;
    cpu6502::CPU cpu;

    virtual void SetUp()
    {
        cpu.Reset(mem);
    }

    virtual void TearDown()
    {
    }
    void ChangeValue(char Operation, Byte &Value);
    void MemoryOperationZeroPageMode(Byte Instruction, char Operation);
    void MemoryOperationZeroPageXMode(Byte Instruction, char Operation);
    void MemoryOperationAbsoluteMode(Byte Instruction, char Operation);
    void MemoryOperationAbsoluteXMode(Byte Instruction, char Operation);
};

void CPU6502IncrementsAndDecrementsTests::ChangeValue(char Operation, Byte &Value)
{
    if (Operation == 'I')
        Value++;
    else if (Operation == 'D')
        Value--;
    else
        Value = 0x0; // Error Case - Not implemented
}

void CPU6502IncrementsAndDecrementsTests::MemoryOperationZeroPageMode(Byte Instruction, char Operation)
{
    // Given
    Byte Value = 0x84;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = Value;
    s32 CyclesExpected = 5;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    ChangeValue(Operation, Value);
    EXPECT_EQ(mem[0x0022], Value);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
}

void CPU6502IncrementsAndDecrementsTests::MemoryOperationZeroPageXMode(Byte Instruction, char Operation)
{
    // Given:
    Byte Value = 0x84;
    cpu.X = 0x50;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x22;
    // 0x0072 = 0x22 + 0x50
    mem[0x0072] = Value;
    s32 CyclesExpected = 6;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    ChangeValue(Operation, Value);
    EXPECT_EQ(mem[0x0072], Value);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
}

void CPU6502IncrementsAndDecrementsTests::MemoryOperationAbsoluteMode(Byte Instruction, char Operation)
{
    // Given:
    Byte Value = 0x84;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;
    mem[0x4480] = Value;
    s32 CyclesExpected = 6;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    ChangeValue(Operation, Value);
    EXPECT_EQ(mem[0x4480], Value);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
}

void CPU6502IncrementsAndDecrementsTests::MemoryOperationAbsoluteXMode(Byte Instruction, char Operation)
{
    // Given:
    Byte Value = 0x84;
    cpu.X = 0x92;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = Value;
    s32 CyclesExpected = 7;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    ChangeValue(Operation, Value);
    EXPECT_EQ(mem[0x2092], Value);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
}

TEST_F(CPU6502IncrementsAndDecrementsTests, IncrementMemoryLocationZeroPageMode)
{
    MemoryOperationZeroPageMode(CPU::INS_INC_ZERO_P, 'I');
}

TEST_F(CPU6502IncrementsAndDecrementsTests, IncrementMemoryLocationZeroXPageMode)
{
    MemoryOperationZeroPageXMode(CPU::INS_INC_ZERO_PX, 'I');
}

TEST_F(CPU6502IncrementsAndDecrementsTests, IncrementMemoryLocationAbsoluteMode)
{
    MemoryOperationAbsoluteMode(CPU::INS_INC_ABS, 'I');
}

TEST_F(CPU6502IncrementsAndDecrementsTests, IncrementMemoryLocationAbsoluteXMode)
{
    MemoryOperationAbsoluteXMode(CPU::INS_INC_ABS_X, 'I');
}

TEST_F(CPU6502IncrementsAndDecrementsTests, DecrementMemoryLocationZeroPageMode)
{
    MemoryOperationZeroPageMode(CPU::INS_DEC_ZERO_P, 'D');
}

TEST_F(CPU6502IncrementsAndDecrementsTests, DecrementMemoryLocationZeroXPageMode)
{
    MemoryOperationZeroPageXMode(CPU::INS_DEC_ZERO_PX, 'D');
}

TEST_F(CPU6502IncrementsAndDecrementsTests, DecrementMemoryLocationAbsoluteMode)
{
    MemoryOperationAbsoluteMode(CPU::INS_DEC_ABS, 'D');
}

TEST_F(CPU6502IncrementsAndDecrementsTests, DecrementMemoryLocationAbsoluteXMode)
{
    MemoryOperationAbsoluteXMode(CPU::INS_DEC_ABS_X, 'D');
}