#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502LoadRegisterTests : public testing::Test
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

    void LoadRegisterImmediateValue(Byte Instruction, Byte CPU::*RegisterToCheck);
    void LoadRegisterZeroPage(Byte Instruction, Byte CPU::*RegisterToCheck);
    void LoadRegisterZeroPageX(Byte Instruction, Byte CPU::*RegisterToCheck);
    void LoadRegisterAbsolute(Byte Instruction, Byte CPU::*RegisterToCheck);
    void LoadRegisterAbsoluteX(Byte Instruction, Byte CPU::*RegisterToCheck);
    void LoadRegisterAbsoluteXCrossingPage(Byte Instruction, Byte CPU::*RegisterToCheck);
    void LoadRegisterAbsoluteY(Byte Instruction, Byte CPU::*RegisterToCheck);
    void LoadRegisterAbsoluteYCrossingPage(Byte Instruction, Byte CPU::*RegisterToCheck);
};

static void VerifyNotAffectedFlags(cpu6502::CPU &cpu, cpu6502::CPU &cpuCopy)
{
    // not affected by LDA, LDX, LDY 
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
}

void CPU6502LoadRegisterTests::LoadRegisterImmediateValue(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x84;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(2, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, 0x84);
    EXPECT_EQ(CyclesUsed, 2);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LoadRegisterTests::LoadRegisterZeroPage(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x22;
    mem[0x0022] = 0x84;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(3, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, 0x84);
    EXPECT_EQ(CyclesUsed, 3);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LoadRegisterTests::LoadRegisterZeroPageX(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    cpu.X = 0x50;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x22;
    // 0x0072 = 0x22 + 0x50
    mem[0x0072] = 0x84;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LoadRegisterTests::LoadRegisterAbsolute(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x80;
    mem[0xFFFE] = 0x44;
    mem[0x4480] = 0x84;

    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LoadRegisterTests::LoadRegisterAbsoluteX(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    cpu.X = 0x92;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = 0x84;

    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LoadRegisterTests::LoadRegisterAbsoluteXCrossingPage(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    cpu.X = 0x1;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0xFF;
    mem[0xFFFE] = 0x44;
    // 0x44FF + 0x1 = 0x4500 -> Crosses page boundary
    mem[0x4500] = 0x84;
    CPU cpuCopy = cpu;
    s32 CyclesExpected = 5;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, 0x84);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LoadRegisterTests::LoadRegisterAbsoluteY(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    cpu.Y = 0x92;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = 0x84;

    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502LoadRegisterTests::LoadRegisterAbsoluteYCrossingPage(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    cpu.Y = 0x1;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0xFF;
    mem[0xFFFE] = 0x44;
    // 0x44FF + 0x1 = 0x4500 -> Crosses page boundary
    mem[0x4500] = 0x84;
    CPU cpuCopy = cpu;
    s32 CyclesExpected = 5;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, 0x84);
    EXPECT_EQ(CyclesUsed, CyclesExpected);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LoadRegisterTests, CPUDoesCompleteInstructionWithLessCycles)
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

TEST_F(CPU6502LoadRegisterTests, LDAImmediateLoadValue)
{
    LoadRegisterImmediateValue(CPU::INS_LDA_IM, &CPU::A);
}

TEST_F(CPU6502LoadRegisterTests, LDXImmediateLoadValue)
{
    LoadRegisterImmediateValue(CPU::INS_LDX_IM, &CPU::X);
}

TEST_F(CPU6502LoadRegisterTests, LDYImmediateLoadValue)
{
    LoadRegisterImmediateValue(CPU::INS_LDY_IM, &CPU::Y);
}

TEST_F(CPU6502LoadRegisterTests, LDAZeroPageLoadValue)
{
    LoadRegisterZeroPage(CPU::INS_LDA_ZEROP, &CPU::A);
}

TEST_F(CPU6502LoadRegisterTests, LDXZeroPageLoadValue)
{
    LoadRegisterZeroPage(CPU::INS_LDX_ZEROP, &CPU::X);
}

TEST_F(CPU6502LoadRegisterTests, LDYZeroPageLoadValue)
{
    LoadRegisterZeroPage(CPU::INS_LDY_ZEROP, &CPU::Y);
}

TEST_F(CPU6502LoadRegisterTests, LDAZeroPageXLoadValue)
{

    LoadRegisterZeroPageX(CPU::INS_LDA_ZEROP_X, &CPU::A);
}

TEST_F(CPU6502LoadRegisterTests, LDAZeroPageXLoadValueWhenWraps)
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

TEST_F(CPU6502LoadRegisterTests, LDXZeroPageYLoadValue)
{
    // Given:
    cpu.Y = 0x50;
    mem[0xFFFC] = CPU::INS_LDX_ZEROP_Y;
    mem[0xFFFD] = 0x22;
    // 0x0072 = 0x22 + 0x50
    mem[0x0072] = 0x84;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.X, 0x84);
    EXPECT_EQ(CyclesUsed, 4);
    EXPECT_TRUE(cpu.N);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502LoadRegisterTests, LDYZeroPageXLoadValue)
{

    LoadRegisterZeroPageX(CPU::INS_LDY_ZEROP_X, &CPU::Y);
}

TEST_F(CPU6502LoadRegisterTests, LDAAbsoluteLoadValue)
{
    LoadRegisterAbsolute(CPU::INS_LDA_ABS, &CPU::A);
}

TEST_F(CPU6502LoadRegisterTests, LDXAbsoluteLoadValue)
{
    LoadRegisterAbsolute(CPU::INS_LDX_ABS, &CPU::X);
}

TEST_F(CPU6502LoadRegisterTests, LDYAbsoluteLoadValue)
{
    LoadRegisterAbsolute(CPU::INS_LDY_ABS, &CPU::Y);
}

TEST_F(CPU6502LoadRegisterTests, LDAAbsoluteXLoadValue)
{
    LoadRegisterAbsoluteX(CPU::INS_LDA_ABS_X, &CPU::A);
}

TEST_F(CPU6502LoadRegisterTests, LDYAbsoluteXLoadValue)
{
    LoadRegisterAbsoluteX(CPU::INS_LDY_ABS_X, &CPU::Y);
}

TEST_F(CPU6502LoadRegisterTests, LDAAbsoluteXLoadValueCrossingPage)
{
    LoadRegisterAbsoluteXCrossingPage(CPU::INS_LDA_ABS_X, &CPU::A);
}

TEST_F(CPU6502LoadRegisterTests, LDYAbsoluteXLoadValueCrossingPage)
{
    LoadRegisterAbsoluteXCrossingPage(CPU::INS_LDY_ABS_X, &CPU::Y);
}

TEST_F(CPU6502LoadRegisterTests, LDAAbsoluteYLoadValue)
{
    LoadRegisterAbsoluteY(CPU::INS_LDA_ABS_Y, &CPU::A);
}

TEST_F(CPU6502LoadRegisterTests, LDXAbsoluteYLoadValue)
{
    LoadRegisterAbsoluteY(CPU::INS_LDX_ABS_Y, &CPU::X);
}

TEST_F(CPU6502LoadRegisterTests, LDAAbsoluteYLoadValueCrossingPage)
{
    LoadRegisterAbsoluteYCrossingPage(CPU::INS_LDA_ABS_Y, &CPU::A);
}

TEST_F(CPU6502LoadRegisterTests, LDXAbsoluteYLoadValueCrossingPage)
{
    LoadRegisterAbsoluteYCrossingPage(CPU::INS_LDX_ABS_Y, &CPU::X);
}

TEST_F(CPU6502LoadRegisterTests, LDAIndirectXLoadAValue)
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

TEST_F(CPU6502LoadRegisterTests, LDAIndirectYLoadAValue)
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

TEST_F(CPU6502LoadRegisterTests, LDAIndirectYLoadAValueCrossingPage)
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