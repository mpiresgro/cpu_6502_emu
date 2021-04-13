
#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502LogicalOperationsTests : public testing::Test
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

TEST_F(CPU6502LogicalOperationsTests, ANDImmediateMode)
{
    // Given:
    cpu.A = 0x0D;
    mem[0xFFFC] = CPU::INS_AND_IM;
    mem[0xFFFD] = 0x0A;
    s32 CyclesExpected = 2;
    // CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x08);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LogicalOperationsTests, ANDZeroPageMode)
{
    // Given:
    cpu.A = 0x0D;
    mem[0xFFFC] = CPU::INS_AND_ZERO_P;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 0x0A;
    // CPU cpuCopy = cpu;
    s32 CyclesExpected = 3;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x08);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // EXPECT_TRUE(cpu.flags.N);
    // VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LogicalOperationsTests, ANDZeroPageXMode)
{
    // Given:
    cpu.X = 0x50;
    cpu.A = 0x0D;
    mem[0xFFFC] = CPU::INS_AND_ZERO_PX;
    mem[0xFFFD] = 0x22;
    // 0x0072 = 0x22 + 0x50
    mem[0x0072] = 0x0A;
    // CPU cpuCopy = cpu;
    s32 CyclesExpected = 4;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x08);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // EXPECT_TRUE(cpu.flags.N);
    // VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LogicalOperationsTests, ANDAbsoluteMode)
{
    // Given:
    cpu.A = 0x0D;
    mem[0xFFFC] = CPU::INS_AND_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;
    mem[0x4480] = 0x0A;
    s32 CyclesExpected = 4;
    // CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x08);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // EXPECT_TRUE(cpu.flags.N);
    // VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LogicalOperationsTests, ANDAbsoluteXMode)
{
    // Given:
    cpu.A = 0x0D;
    cpu.X = 0x92;
    mem[0xFFFC] = CPU::INS_AND_ABS_X;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = 0x0A;
    s32 CyclesExpected = 4;
    // CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x08);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // EXPECT_TRUE(cpu.flags.N);
    // VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LogicalOperationsTests, ANDAbsoluteYMode)
{
    // Given:
    cpu.A = 0x0D;
    cpu.Y = 0x92;
    mem[0xFFFC] = CPU::INS_AND_ABS_Y;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = 0x0A;
    s32 CyclesExpected = 4;
    // CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x08);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    // EXPECT_TRUE(cpu.flags.N);
    // VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LogicalOperationsTests, ANDIndirectXMode)
{
    // given:
    cpu.A = 0x0D;
    cpu.X = 0x04;
    mem[0xFFFC] = CPU::INS_AND_IND_X;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; //0x2 + 0x4
    mem[0x0007] = 0x80;
    mem[0x8000] = 0x0A;
    constexpr s32 EXPECTED_CYCLES = 6;

    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    //then:
    EXPECT_EQ(cpu.A, 0x08);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    // VerifyNotAffectedFlags(cpu, CPUCopy);
}

TEST_F(CPU6502LogicalOperationsTests, ANDIndirectYMode)
{
    // given:
    cpu.A = 0x0D;
    cpu.Y = 0x04;
    mem[0xFFFC] = CPU::INS_AND_IND_Y;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8004] = 0x0A; //0x8000 + 0x4
    constexpr s32 EXPECTED_CYCLES = 5;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    //then:
    EXPECT_EQ(cpu.A, 0x08);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
}