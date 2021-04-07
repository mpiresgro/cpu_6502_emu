#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502Test1 : public testing::Test
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
    constexpr s32 NUM_OF_CYCLES = 0;
    // When:
    s32 CyclesUsed = cpu.Execute(NUM_OF_CYCLES, mem);
    // Then
    EXPECT_EQ(CyclesUsed, 0);
}

TEST_F(CPU6502Test1, CPUDoesCompleteInstructionWithLessCycles)
{
    // Given:
    mem[0xFFFC] = CPU::INS_LDA_IM;
    mem[0xFFFD] = 0x84;
    constexpr s32 NUM_OF_CYCLES = 1;
    // When:
    s32 CyclesUsed = cpu.Execute(NUM_OF_CYCLES, mem);
    // Then:
    // INS_LDA_IM requires 2 cycles
    EXPECT_EQ(CyclesUsed, 2);
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
    // The address calculation wraps around if the sum of the base address and the register exceed 0xFF.
    //  If we repeat the last example but with 0xFF in the X register then the accumulator will be
    //  loaded from 0x007F (e.g. 0x80 + 0xFF => 0x7F) and not 0x017F.

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

TEST_F(CPU6502Test1, LDAAbsoluteLoadValue)
{
    // Given:
    mem[0xFFFC] = CPU::INS_LDA_ABS;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;
    mem[0x4480] = 0x84;

    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502Test1, LDAAbsoluteXLoadValue)
{
    // Given:
    cpu.X = 0x92;
    mem[0xFFFC] = CPU::INS_LDA_ABS_X;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = 0x84;

    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502Test1, LDAAbsoluteXLoadValueCrossingPage)
{
    // Given:
    cpu.X = 0x1;
    mem[0xFFFC] = CPU::INS_LDA_ABS_X;
    mem[0xFFFD] = 0xFF;
    mem[0xFFFE] = 0x44;
    // 0x44FF + 0x1 = 0x4500 -> Crosses page boundary
    mem[0x4500] = 0x84;
    CPU cpuCopy = cpu;
    s32 CyclesExpected = 5;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502Test1, LDAAbsoluteYLoadValue)
{
    // Given:
    cpu.Y = 0x92;
    mem[0xFFFC] = CPU::INS_LDA_ABS_Y;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = 0x84;

    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502Test1, LDAAbsoluteYLoadValueCrossingPage)
{
    // Given:
    cpu.Y = 0x1;
    mem[0xFFFC] = CPU::INS_LDA_ABS_Y;
    mem[0xFFFD] = 0xFF;
    mem[0xFFFE] = 0x44;
    // 0x44FF + 0x1 = 0x4500 -> Crosses page boundary
    mem[0x4500] = 0x84;
    CPU cpuCopy = cpu;
    s32 CyclesExpected = 5;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x84);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502Test1, LDAIndirectXLoadAValue)
{
    // given:
    cpu.X = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_IND_X;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; //0x2 + 0x4
    mem[0x0007] = 0x80;
    mem[0x8000] = 0x37;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    //then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}

TEST_F(CPU6502Test1, LDAIndirectYLoadAValue)
{
    // given:
    cpu.Y = 0x04;
    mem[0xFFFC] = CPU::INS_LDA_IND_Y;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8004] = 0x37; //0x8000 + 0x4
    constexpr s32 EXPECTED_CYCLES = 5;
    CPU CPUCopy = cpu;

    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    //then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}

TEST_F(CPU6502Test1, LDAIndirectYLoadAValueCrossingPage)
{
    // given:
    cpu.Y = 0xFF;
    mem[0xFFFC] = CPU::INS_LDA_IND_Y;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x02;
    mem[0x0003] = 0x80;
    mem[0x8101] = 0x37; //0x8002 + 0xFF
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    //then:
    EXPECT_EQ(cpu.A, 0x37);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.Z);
    EXPECT_FALSE(cpu.N);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}