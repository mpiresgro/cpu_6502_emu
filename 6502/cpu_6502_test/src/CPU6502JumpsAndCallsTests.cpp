#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502JumpsAndCallsTests : public testing::Test
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
    // not affected by JMP, JSR, RTS
    EXPECT_EQ(cpu.flags.C, cpuCopy.flags.C);
    EXPECT_EQ(cpu.flags.I, cpuCopy.flags.I);
    EXPECT_EQ(cpu.flags.D, cpuCopy.flags.D);
    EXPECT_EQ(cpu.flags.B, cpuCopy.flags.B);
    EXPECT_EQ(cpu.flags.V, cpuCopy.flags.V);
}

TEST_F(CPU6502JumpsAndCallsTests, JumpToSubroutineAndJumpBack)
{
    // Given:
    cpu.Reset(mem, 0xFF00);
    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x44;
    mem[0xFF02] = 0x20;
    // 0x2044 -> Target Location
    mem[0x2044] = CPU::INS_RTS;
    mem[0xFF03] = CPU::INS_LDA_IM;
    mem[0xFF04] = 0x50;
    // INS_JSR + INS_RTS + LDA_IM
    s32 CyclesExpected = 6 + 6 + 2;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x50);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502JumpsAndCallsTests, JSRDoesntAffectProcessorStatus)
{
    // Given:
    cpu.Reset(mem, 0xFF00);
    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x44;
    mem[0xFF02] = 0x20;
    s32 CyclesExpected = 6;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_NE(cpu.SP, cpuCopy.SP);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
}

TEST_F(CPU6502JumpsAndCallsTests, RTSDoesntAffectProcessorStatus)
{
    // Given:
    cpu.Reset(mem, 0xFF00);
    mem[0xFF00] = CPU::INS_JSR;
    mem[0xFF01] = 0x44;
    mem[0xFF02] = 0x20;
    mem[0x2044] = CPU::INS_RTS;
    s32 CyclesExpected = 6 + 6;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
}

TEST_F(CPU6502JumpsAndCallsTests, JumpToLocationAbsolute)
{
    // Given:
    mem[0xFFFC] = CPU::INS_JMP_ABS;
    mem[0xFFFD] = 0x44;
    mem[0xFFFE] = 0x20;
    mem[0x2044] = CPU::INS_LDA_IM;
    mem[0x2045] = 0x50;
    // INS_JMP_ABS + INS_LDA_IM
    s32 CyclesExpected = 3 + 2;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x50);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}

TEST_F(CPU6502JumpsAndCallsTests, JumpToLocationAIndirect)
{
    mem[0xFFFC] = CPU::INS_JMP_IND;
    mem[0xFFFD] = 0x44;
    mem[0xFFFE] = 0x20;
    mem[0x2044] = 0x30;
    mem[0x2045] = 0x02;
    mem[0x0230] = CPU::INS_LDA_IM;
    mem[0x0231] = 0x50;
    // INS_JMP_IND + INS_LDA_IM
    s32 CyclesExpected = 5 + 2;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.A, 0x50);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_EQ(cpu.SP, cpuCopy.SP);
    EXPECT_EQ(cpu.PS, cpuCopy.PS);
}