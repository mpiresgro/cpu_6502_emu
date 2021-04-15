
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
    
    void TransferAccumulatorToRegister(Byte Instruction, Byte CPU::*Register);
    void TransferRegisterToAccumulator(Byte Instruction, Byte CPU::*Register);
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

void CPU6502RegisterTransferTests::TransferAccumulatorToRegister(Byte Instruction, Byte CPU::*Register){
    // given:
    cpu.A = 0x20;
    cpu.*Register = 0x40;
    mem[0xFFFC] = Instruction;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    //then:
    EXPECT_EQ(cpu.A, 0x20);
    EXPECT_EQ(cpu.*Register, 0x20);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}

void CPU6502RegisterTransferTests::TransferRegisterToAccumulator(Byte Instruction, Byte CPU::*Register){
    // given:
    cpu.A = 0x20;
    cpu.*Register = 0x40;
    mem[0xFFFC] = Instruction;
    constexpr s32 EXPECTED_CYCLES = 2;
    CPU CPUCopy = cpu;
    //when:
    s32 CyclesUsed = cpu.Execute(EXPECTED_CYCLES, mem);
    //then:
    EXPECT_EQ(cpu.A, 0x40);
    EXPECT_EQ(cpu.*Register, 0x40);
    EXPECT_EQ(CyclesUsed, EXPECTED_CYCLES);
    VerifyNotAffectedFlags(cpu, CPUCopy);
}

TEST_F(CPU6502RegisterTransferTests, TransferAccumulatorToX)
{
    TransferAccumulatorToRegister(CPU::INS_TAX, &CPU::X);
}

TEST_F(CPU6502RegisterTransferTests, TransferAccumulatorToY)
{
    TransferAccumulatorToRegister(CPU::INS_TAY, &CPU::Y);
}

TEST_F(CPU6502RegisterTransferTests, TransferXToAccumulator)
{
    TransferRegisterToAccumulator(CPU::INS_TXA, &CPU::X);
}

TEST_F(CPU6502RegisterTransferTests, TransferYToAccumulator)
{
    TransferRegisterToAccumulator(CPU::INS_TYA, &CPU::Y);
}