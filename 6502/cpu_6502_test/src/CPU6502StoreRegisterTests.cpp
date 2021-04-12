#include <gtest/gtest.h>
#include "main_6502.h"

using namespace cpu6502;

class CPU6502StoreRegisterTests : public testing::Test
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

    void StoreRegisterZeroPage(Byte Instruction, Byte CPU::*RegisterToCheck);
    void StoreRegisterZeroPageX(Byte Instruction, Byte CPU::*RegisterToCheck);
    void StoreRegisterAbsolute(Byte Instruction, Byte CPU::*RegisterToCheck);
    void StoreRegisterAbsoluteWithOffset(Byte Instruction, Byte CPU::*RegisterToCheck, Byte CPU::*Offset);
};

static void VerifyNotAffectedFlags(cpu6502::CPU &cpu, cpu6502::CPU &cpuCopy)
{
    // not affected by STA, STX, STY
    EXPECT_EQ(cpu.flags.C, cpuCopy.flags.C);
    EXPECT_EQ(cpu.flags.Z, cpuCopy.flags.Z);
    EXPECT_EQ(cpu.flags.I, cpuCopy.flags.I);
    EXPECT_EQ(cpu.flags.D, cpuCopy.flags.D);
    EXPECT_EQ(cpu.flags.B, cpuCopy.flags.B);
    EXPECT_EQ(cpu.flags.V, cpuCopy.flags.V);
    EXPECT_EQ(cpu.flags.N, cpuCopy.flags.N);
}

void CPU6502StoreRegisterTests::StoreRegisterZeroPage(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    cpu.*RegisterToCheck = 0x20;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x79;
    mem[0x0079] = 0x10;
    CPU cpuCopy = cpu;
    s32 CyclesExpected = 3;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, mem[0x0079]);
    EXPECT_EQ(CyclesUsed, CyclesExpected);

    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502StoreRegisterTests::StoreRegisterZeroPageX(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    cpu.*RegisterToCheck = 0x20;
    cpu.X = 0x50;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x22;
    // 0x0072 = 0x22 + 0x50
    mem[0x0072] = 0x10;
    CPU cpuCopy = cpu;
    s32 CyclesExpected = 4;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, mem[0x0072]);
    EXPECT_EQ(CyclesUsed, CyclesExpected);

    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502StoreRegisterTests::StoreRegisterAbsolute(Byte Instruction, Byte CPU::*RegisterToCheck)
{
    // Given:
    cpu.*RegisterToCheck = 0x20;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x44;
    mem[0xFFFE] = 0x20;
    mem[0x2044] = 0x10;
    s32 CyclesExpected = 4;
    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(CyclesExpected, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, mem[0x2044]);
    EXPECT_EQ(CyclesUsed, CyclesExpected);

    VerifyNotAffectedFlags(cpu, cpuCopy);
}

void CPU6502StoreRegisterTests::StoreRegisterAbsoluteWithOffset(Byte Instruction, Byte CPU::*RegisterToCheck, Byte CPU::*Offset)
{
    // Given:
    cpu.*RegisterToCheck = 0x20;
    cpu.*Offset = 0x92;
    mem[0xFFFC] = Instruction;
    mem[0xFFFD] = 0x00;
    mem[0xFFFE] = 0x20;
    // 0x2092 (e.g. 0x2000 + 0x92)
    mem[0x2092] = 0x10;

    CPU cpuCopy = cpu;
    // When:
    s32 CyclesUsed = cpu.Execute(4, mem);
    // Then:
    EXPECT_EQ(cpu.*RegisterToCheck, mem[0x2092]);
    EXPECT_EQ(CyclesUsed, 4);
    VerifyNotAffectedFlags(cpu, cpuCopy);
}

TEST_F(CPU6502StoreRegisterTests, STAZeroPageStoreValue)
{
    StoreRegisterZeroPage(CPU::INS_STA_ZEROP, &CPU::A);
}

TEST_F(CPU6502StoreRegisterTests, STXZeroPageStoreValue)
{
    StoreRegisterZeroPage(CPU::INS_STX_ZEROP, &CPU::X);
}

TEST_F(CPU6502StoreRegisterTests, STYZeroPageStoreValue)
{
    StoreRegisterZeroPage(CPU::INS_STY_ZEROP, &CPU::Y);
}

TEST_F(CPU6502StoreRegisterTests, STAZeroPageXStoreValue)
{
    StoreRegisterZeroPageX(CPU::INS_STA_ZEROP_X, &CPU::A);
}

TEST_F(CPU6502StoreRegisterTests, STYZeroPageXStoreValue)
{
    StoreRegisterZeroPageX(CPU::INS_STY_ZEROP_X, &CPU::Y);
}

// TODO: STX ZeroPage Y StoreValue

TEST_F(CPU6502StoreRegisterTests, STAAbsoluteStoreValue)
{
    StoreRegisterAbsolute(CPU::INS_STA_ABS, &CPU::A);
}

TEST_F(CPU6502StoreRegisterTests, STXAbsoluteStoreValue)
{
    StoreRegisterAbsolute(CPU::INS_STX_ABS, &CPU::X);
}

TEST_F(CPU6502StoreRegisterTests, STYAbsoluteStoreValue)
{
    StoreRegisterAbsolute(CPU::INS_STY_ABS, &CPU::Y);
}

TEST_F(CPU6502StoreRegisterTests, STAAbsoluteXStoreValue)
{
    StoreRegisterAbsoluteWithOffset(CPU::INS_STA_ABS_X, &CPU::A, &CPU::X);
}

TEST_F(CPU6502StoreRegisterTests, STAAbsoluteYStoreValue)
{
    StoreRegisterAbsoluteWithOffset(CPU::INS_STA_ABS_Y, &CPU::A, &CPU::Y);
}


TEST_F(CPU6502StoreRegisterTests, STAIndirectXLoadAValue)
{
    // given:
    cpu.A = 0x20;
    cpu.X = 0x04;
    mem[0xFFFC] = CPU::INS_STA_IND_X;
    mem[0xFFFD] = 0x02;
    mem[0x0006] = 0x00; //0x2 + 0x4
    mem[0x0007] = 0x80;
    mem[0x8000] = 0x10;
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    //then:
    EXPECT_EQ(cpu.A, mem[0x8000]);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.flags.Z);
    EXPECT_FALSE(cpu.flags.N);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}

TEST_F(CPU6502StoreRegisterTests, STAIndirectYLoadAValue)
{
    // given:
    cpu.A = 0x20;
    cpu.Y = 0x04;
    mem[0xFFFC] = CPU::INS_STA_IND_Y;
    mem[0xFFFD] = 0x02;
    mem[0x0002] = 0x00;
    mem[0x0003] = 0x80;
    mem[0x8004] = 0x10; //0x8000 + 0x4
    constexpr s32 EXPECTED_CYCLES = 6;
    CPU CPUCopy = cpu;

    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);

    //then:
    EXPECT_EQ(cpu.A, mem[0x8004]);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    EXPECT_FALSE(cpu.flags.Z);
    EXPECT_FALSE(cpu.flags.N);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}