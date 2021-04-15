
#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502RegisterTransferTests : public testing::Test
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
    // not affected by Tranfer Register Operations
    EXPECT_EQ(cpu.flags.C, cpuCopy.flags.C);
    EXPECT_EQ(cpu.flags.I, cpuCopy.flags.I);
    EXPECT_EQ(cpu.flags.D, cpuCopy.flags.D);
    EXPECT_EQ(cpu.flags.B, cpuCopy.flags.B);
    EXPECT_EQ(cpu.flags.V, cpuCopy.flags.V);
}

TEST_F(CPU6502RegisterTransferTests, TransferAccumulatorToX)
{
    // given:
    cpu.A = 0x20;
    cpu.X = 0x40;
    mem[0xFFFC] = CPU::INS_TAX;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    //then:
    EXPECT_EQ(cpu.A, 0x20);
    EXPECT_EQ(cpu.X, 0x20);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}

TEST_F(CPU6502RegisterTransferTests, TransferAccumulatorToY)
{
    // given:
    cpu.A = 0x20;
    cpu.Y = 0x40;
    mem[0xFFFC] = CPU::INS_TAY;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    //then:
    EXPECT_EQ(cpu.A, 0x20);
    EXPECT_EQ(cpu.Y, 0x20);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}

TEST_F(CPU6502RegisterTransferTests, TransferXToAccumulator)
{
    // given:
    cpu.A = 0x20;
    cpu.X = 0x40;
    mem[0xFFFC] = CPU::INS_TXA;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    //then:
    EXPECT_EQ(cpu.A, 0x40);
    EXPECT_EQ(cpu.X, 0x40);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}

TEST_F(CPU6502RegisterTransferTests, TransferYToAccumulator)
{
    // given:
    cpu.A = 0x20;
    cpu.Y = 0x40;
    mem[0xFFFC] = CPU::INS_TYA;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    //then:
    EXPECT_EQ(cpu.A, 0x40);
    EXPECT_EQ(cpu.Y, 0x40);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}