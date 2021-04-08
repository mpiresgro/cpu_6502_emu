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

void VerifyNotAffectedFlags(cpu6502::CPU &cpu, cpu6502::CPU &cpuCopy)
{
    // not affected by STA, STX, STY
    EXPECT_EQ(cpu.C, cpuCopy.C);
    EXPECT_EQ(cpu.Z, cpuCopy.Z);
    EXPECT_EQ(cpu.I, cpuCopy.I);
    EXPECT_EQ(cpu.D, cpuCopy.D);
    EXPECT_EQ(cpu.B, cpuCopy.B);
    EXPECT_EQ(cpu.V, cpuCopy.V);
    EXPECT_EQ(cpu.N, cpuCopy.N);
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