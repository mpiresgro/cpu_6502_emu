
#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502LogicalOperationsTests : public testing::Test
{
public:
    cpu6502::Mem mem;
    cpu6502::CPU cpu;
    cpu6502::Byte A = 0x0D;
    cpu6502::Byte B = 0x0A;

    virtual void SetUp()
    {
        cpu.Reset(mem);
    }

    virtual void TearDown()
    {
    }

    Byte GetLogicOpValue(char Operator);
    void LogicOpImmediateMode(Byte Instruction, char Operator);
    void LogicOpZeroPageMode(Byte Instruction, char Operator);
    void LogicOpZeroPageXMode(Byte Instruction, char Operator);
    void LogicOpAbsoluteMode(Byte Instruction, char Operator);
    void LogicOpAbsoluteOffsetMode(Byte Instruction, char Operator, Byte CPU::*RegisterOffset);
    void LogicOpIndirectXMode(Byte Instruction, char Operator);
    void LogicOpIndirectYMode(Byte Instruction, char Operator);
};

static void VerifyNotAffectedFlags(cpu6502::CPU &cpu, cpu6502::CPU &cpuCopy)
{
    // not affected by LogicOperations
    EXPECT_EQ(cpu.flags.C, cpuCopy.flags.C);
    EXPECT_EQ(cpu.flags.I, cpuCopy.flags.I);
    EXPECT_EQ(cpu.flags.D, cpuCopy.flags.D);
    EXPECT_EQ(cpu.flags.B, cpuCopy.flags.B);
    EXPECT_EQ(cpu.flags.V, cpuCopy.flags.V);
}

Byte CPU6502LogicalOperationsTests::GetLogicOpValue(char Operator)
{
    switch (Operator)
    {
    case '&':
        return A & B;
        break;
    case '|':
        return A | B;
        break;
    case '^':
        return A ^ B;
        break;
    default:
        // Operator Not Implemented
        return -1;
    }
}

void CPU6502LogicalOperationsTests::LogicOpImmediateMode(Byte Instruction, char Operator)
{
    // Given:
    cpu.A = A;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = B;
    s32 CyclesExpected = 2;
    // CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    Byte Result = GetLogicOpValue(Operator);
    EXPECT_EQ(cpu.A, Result);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LogicalOperationsTests::LogicOpZeroPageMode(Byte Instruction, char Operator)
{
    // Given:
    cpu.A = A;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = B;
    CPU cpuCopy = cpu;
    constexpr s32 CyclesExpected = 3;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    Byte Result = GetLogicOpValue(Operator);
    EXPECT_EQ(cpu.A, Result);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LogicalOperationsTests::LogicOpZeroPageXMode(Byte Instruction, char Operator)
{
    // Given:
    cpu.X = 0x50;
    cpu.A = A;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x22;
    // 0x0072 = 0x22 + 0x50
    mem[0x0072] = B;
    CPU cpuCopy = cpu;
    constexpr s32 CyclesExpected = 4;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    Byte Result = GetLogicOpValue(Operator);
    EXPECT_EQ(cpu.A, Result);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LogicalOperationsTests::LogicOpAbsoluteMode(Byte Instruction, char Operator)
{
    // Given:
    cpu.A = A;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;
    mem[0x4480] = B;
    constexpr s32 CyclesExpected = 4;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    Byte Result = GetLogicOpValue(Operator);
    EXPECT_EQ(cpu.A, Result);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LogicalOperationsTests::LogicOpAbsoluteOffsetMode(Byte Instruction, char Operator, Byte CPU::*RegisterOffset)
{
    // Given:
    cpu.A = A;
    cpu.*RegisterOffset = 0x92;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = B;
    constexpr s32 CyclesExpected = 4;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    Byte Result = GetLogicOpValue(Operator);
    EXPECT_EQ(cpu.A, Result);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LogicalOperationsTests::LogicOpIndirectXMode(Byte Instruction, char Operator)
{
    // given:
    cpu.A = A;
    cpu.X = 0x04;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; //0x2 + 0x4
    mem[0x0007] = 0x80;
    mem[0x8000] = B;
    constexpr s32 EXPECTED_CYCLES = 6;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    //then:
    Byte Result = GetLogicOpValue(Operator);
    EXPECT_EQ(cpu.A, Result);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    // VerifyNotAffectedFlags(cpu, CPUCopy);
}

void CPU6502LogicalOperationsTests::LogicOpIndirectYMode(Byte Instruction, char Operator)
{
    // given:
    cpu.A = A;
    cpu.Y = 0x04;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8004] = 0x0A; //0x8000 + 0x4
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU cpuCopy = cpu;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    //then:
    Byte Result = GetLogicOpValue(Operator);
    EXPECT_EQ(cpu.A, Result);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LogicalOperationsTests, ANDImmediateMode)
{
    LogicOpImmediateMode(CPU::INS_AND_IM, '&');
}

TEST_F(CPU6502LogicalOperationsTests, ANDZeroPageMode)
{
    LogicOpZeroPageMode(CPU::INS_AND_ZERO_P, '&');
}

TEST_F(CPU6502LogicalOperationsTests, ANDZeroPageXMode)
{
    LogicOpZeroPageXMode(CPU::INS_AND_ZERO_PX, '&');
}

TEST_F(CPU6502LogicalOperationsTests, ANDAbsoluteMode)
{
    LogicOpAbsoluteMode(CPU::INS_AND_ABS, '&');
}

TEST_F(CPU6502LogicalOperationsTests, ANDAbsoluteXMode)
{
    LogicOpAbsoluteOffsetMode(CPU::INS_AND_ABS_X, '&', &CPU::X);
}

TEST_F(CPU6502LogicalOperationsTests, ANDAbsoluteYMode)
{
    LogicOpAbsoluteOffsetMode(CPU::INS_AND_ABS_Y, '&', &CPU::Y);
}

TEST_F(CPU6502LogicalOperationsTests, ANDIndirectXMode)
{
    LogicOpIndirectXMode(CPU::INS_AND_IND_X, '&');
}

TEST_F(CPU6502LogicalOperationsTests, ANDIndirectYMode)
{
    LogicOpIndirectYMode(CPU::INS_AND_IND_Y, '&');
}