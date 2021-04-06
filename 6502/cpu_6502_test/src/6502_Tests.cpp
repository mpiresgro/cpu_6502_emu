#include <gtest/gtest.h>
#include "main_6502.h"

class CPU6502Test1 : public testing::Test
{
public:
    Mem mem;
    CPU cpu;

    virtual void SetUp()
    {
        cpu.Reset(mem);
    }

    virtual void TearDown()
    {
    }
};

static void VerifyNotAffectedFlags(CPU &cpu, CPU &cpuCopy)
{
    // not affected by LDA
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
}

TEST_F(CPU6502Test1, CPUKeepsStateWithZeroCyles)
{
    // Given
    CPU cpuCopy = cpu;
    constexpr s32 NUM_OF_CYCLES = 0;
    // When:
    s32 CyclesUsed = cpu.Execute(NUM_OF_CYCLES, mem);
    // Then
    EXPECT_EQ(CyclesUsed, 0);
}

TEST_F(CPU6502Test1, LDAImmediateLoadValue)
{
    // Given:
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(2, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, 2);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502Test1, LDAZeroPageLoadValue)
{
    // Given:
    mem[0xFFFC] = CPU::INS_LDA_ZEROP;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 0x84;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(3, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, 3);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502Test1, LDAZeroPageXLoadValue)
{
    // Given:
    cpu.X = 0x50;
    mem[0xFFFC] = CPU::INS_LDA_ZEROP_X;
    mem[0xFFFD] = 0x22;
    // 0x0072 = 0x22 + 0x50
    mem[0x0072] = 0x84;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502Test1, LDAZeroPageXLoadValueWhenWraps)
{
    // The address calculation wraps around if the sum of the base address and the register exceed $FF.
    //  If we repeat the last example but with $FF in the X register then the accumulator will be
    //  loaded from $007F (e.g. $80 + $FF => $7F) and not $017F.

    // Given:
    cpu.X = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_ZEROP_X;
    mem[0xFFFD] = 0x80;
    mem[0x007F] = 0x84;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}
