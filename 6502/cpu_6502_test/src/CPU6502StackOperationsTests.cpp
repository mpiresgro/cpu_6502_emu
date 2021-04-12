#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502StackOperationsTests : public testing::Test
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
};

static void VerifyNotAffectedFlags(cpu6502::CPU &cpu, cpu6502::CPU &cpuCopy)
{
    // not affected by INS_TSX
    EXPECT_EQ(cpu.flags.C, cpuCopy.flags.C);
    EXPECT_EQ(cpu.flags.I, cpuCopy.flags.I);
    EXPECT_EQ(cpu.flags.D, cpuCopy.flags.D);
    EXPECT_EQ(cpu.flags.B, cpuCopy.flags.B);
    EXPECT_EQ(cpu.flags.V, cpuCopy.flags.V);
}

TEST_F(CPU6502StackOperationsTests, TransferStackPointerToX)
{
    // Given:
    cpu.SP = 0xFA;
    mem[0xFFFC] = CPU::INS_TSX;
    s32 CyclesExpected = 2;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_EQ(cpu.X, 0xFA);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502StackOperationsTests, TransferXToStackPointer)
{
    // Given:
    cpu.X = 0x50;
    mem[0xFFFC] = CPU::INS_TXS;
    s32 CyclesExpected = 2;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_EQ(cpu.SP, 0x50);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(CPU6502StackOperationsTests, TransferAccumulatorIntoStack)
{
    // Given:
    cpu.A = 0x50;
    mem[0xFFFC] = CPU::INS_PHA;
    s32 CyclesExpected = 3;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // Accesing Stack location
    EXPECT_EQ(mem[0x100 | (cpu.SP + 1)], cpu.A);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502StackOperationsTests, TransferStatusFlagsIntoStack)
{
    // Given:
    // Processor Status
    cpu.PS = 0x59;
    mem[0xFFFC] = CPU::INS_PHP;
    s32 CyclesExpected = 3;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // Accesing Stack location
    EXPECT_EQ(mem[0x100 | (cpu.SP + 1)], cpu.PS);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502StackOperationsTests, PullAccumulatorFromStack)
{
    // Given:
    cpu.SP = 0xFE;
    mem[0x1FF] = 0x50;
    mem[0xFFFC] = CPU::INS_PLA;
    s32 CyclesExpected = 4;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_EQ(cpu.A, 0x50);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}   

TEST_F(CPU6502StackOperationsTests, PullProcessorStatusFromStack)
{
    // Given:
    cpu.SP = 0xFE;
    mem[0x1FF] = 0x59;
    mem[0xFFFC] = CPU::INS_PLP;
    s32 CyclesExpected = 4;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_EQ(cpu.PS, 0x59);
    EXPECT_NE(cpu.PS, cpuCopy.PS);
}
