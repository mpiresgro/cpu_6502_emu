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


void VerifyNotAffectedFlags(cpu6502::CPU &cpu, cpu6502::CPU &cpuCopy)
{
    // not affected by JMP, JSR, RTS
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.Z, cpuCopy.Z);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
    EXPECT_EQ(cpu.N, cpuCopy.N);
}


TEST_F(CPU6502JumpsAndCallsTests, JumpToSubroutineAndJumpBack)
{
    // Given:
    cpu.Reset(mem, )
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x44;
    mem[0xFFFE] = 0x20;
    // 0x2044 -> Target Location
    mem[0x2044] = CPU::INS_RTS;

    s32 CyclesExpected = 6;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:

    EXPECT_EQ(CyclesUsed, CyclesExpected);

    VerifyNotAffectedFlags(cpu, cpuCopy);

}