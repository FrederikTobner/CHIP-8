#include <gtest/gtest.h>

#include "virtual_machine_test.h"

namespace {

TEST_F(VirtualMachineTest, InitResetsRegistersAndTimers) {
    EXPECT_EQ(0, vm.I);
    EXPECT_EQ(0, vm.delayTimer);
    EXPECT_EQ(0, vm.soundTimer);
    EXPECT_EQ(0, Instruction());
    EXPECT_EQ(0, vm.currentOpcode);
    for (int i = 0; i < 16; i++) {
        EXPECT_EQ(0, vm.V[i]);
    }
}

TEST_F(VirtualMachineTest, DefaultSpeedIsTenInstructionsPerFrame) {
    EXPECT_EQ(10, vm.instructionsPerFrame);
}

TEST_F(VirtualMachineTest, SetSpeedRoundsToWholeInstructionsPerFrame) {
    virtual_machine_set_speed(&vm, 720);
    EXPECT_EQ(12, vm.instructionsPerFrame);
    virtual_machine_set_speed(&vm, 629);
    EXPECT_EQ(10, vm.instructionsPerFrame);
    virtual_machine_set_speed(&vm, 631);
    EXPECT_EQ(11, vm.instructionsPerFrame);
}

TEST_F(VirtualMachineTest, SetSpeedExecutesAtLeastOneInstructionPerFrame) {
    virtual_machine_set_speed(&vm, 1);
    EXPECT_EQ(1, vm.instructionsPerFrame);
    virtual_machine_set_speed(&vm, 0);
    EXPECT_EQ(1, vm.instructionsPerFrame);
}

TEST_F(VirtualMachineTest, StepExecutesOneInstruction) {
    Write(0, 0x6A2B); // V[A] = 0x2B
    Write(1, 0x6B03); // V[B] = 0x03
    EXPECT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(0x2B, vm.V[0xA]);
    EXPECT_EQ(0, vm.V[0xB]);
    EXPECT_EQ(1, Instruction());
    EXPECT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(0x03, vm.V[0xB]);
    EXPECT_EQ(2, Instruction());
}

TEST_F(VirtualMachineTest, StepFollowsJumps) {
    Write(0, 0x1208); // jump to 0x208 (instruction 4)
    Write(4, 0x6101); // V[1] = 1
    EXPECT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(4, Instruction());
    EXPECT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(1, vm.V[1]);
}

TEST_F(VirtualMachineTest, InstructionsCanStartAtOddAddresses) {
    uint8_t const program[] = {
        0x12, 0x03,       // 0x200: JMP 0x203
        0xFF,             // 0x202: data
        0x61, 0x2A,       // 0x203: V1 = 0x2A
        0x22, 0x09,       // 0x205: CAL 0x209
        0x62, 0x07,       // 0x207: V2 = 7
        0x63, 0x05,       // 0x209: V3 = 5
        0x00, 0xEE,       // 0x20B: RET
    };
    memcpy(vm.memory + kProgramStart, program, sizeof(program));
    for (int i = 0; i < 6; i++) {
        ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0)) << i;
    }
    EXPECT_EQ(0x2A, vm.V[1]);
    EXPECT_EQ(5, vm.V[3]);
    EXPECT_EQ(7, vm.V[2]);
    EXPECT_EQ(0x209, vm.programCounter);
}

TEST_F(VirtualMachineTest, StepSkipsInstructionOnEqualRegisterValue) {
    Write(0, 0x3000); // skip next if V[0] == 0
    Write(1, 0x6101); // skipped
    Write(2, 0x6202); // V[2] = 2
    EXPECT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(0, vm.V[1]);
    EXPECT_EQ(2, vm.V[2]);
}

TEST_F(VirtualMachineTest, ExitOpcodeHaltsTheMachine) {
    Write(0, 0x0002);
    EXPECT_EQ(VM_HALT, virtual_machine_step(&vm, 0));
}

TEST_F(VirtualMachineTest, EndOfProgramHaltsTheMachine) {
    EXPECT_EQ(VM_HALT, virtual_machine_step(&vm, 0));
}

TEST_F(VirtualMachineTest, EndOfMemoryHaltsTheMachine) {
    vm.programCounter = 0x1000;
    EXPECT_EQ(VM_HALT, virtual_machine_step(&vm, 0));
}

TEST_F(VirtualMachineTest, UnknownOpcodeIsAnError) {
    Write(0, 0x5001); // 5XY0 requires the last nibble to be 0
    EXPECT_EQ(VM_ERROR, virtual_machine_step(&vm, 0));
}

TEST_F(VirtualMachineTest, RunFrameExecutesTheConfiguredAmountOfInstructions) {
    for (int i = 0; i < 40; i++) {
        Write(i, 0x0001); // NOP
    }
    EXPECT_EQ(VM_OK, virtual_machine_run_frame(&vm, 0));
    EXPECT_EQ(10, Instruction());
    virtual_machine_set_speed(&vm, 1200);
    EXPECT_EQ(VM_OK, virtual_machine_run_frame(&vm, 0));
    EXPECT_EQ(30, Instruction());
}

TEST_F(VirtualMachineTest, RunFrameTicksTimersOncePerFrame) {
    for (int i = 0; i < 40; i++) {
        Write(i, 0x0001); // NOP
    }
    vm.delayTimer = 3;
    EXPECT_EQ(VM_OK, virtual_machine_run_frame(&vm, 0));
    EXPECT_EQ(2, vm.delayTimer);
    EXPECT_EQ(VM_OK, virtual_machine_run_frame(&vm, 0));
    EXPECT_EQ(1, vm.delayTimer);
}

TEST_F(VirtualMachineTest, RunFrameDoesNotUnderflowTimers) {
    for (int i = 0; i < 20; i++) {
        Write(i, 0x0001); // NOP
    }
    EXPECT_EQ(VM_OK, virtual_machine_run_frame(&vm, 0));
    EXPECT_EQ(0, vm.delayTimer);
}

TEST_F(VirtualMachineTest, RunFrameStopsWhenProgramIsFinished) {
    Write(0, 0x0001); // NOP
    Write(1, 0x0002); // EXT
    EXPECT_EQ(VM_HALT, virtual_machine_run_frame(&vm, 0));
}

TEST_F(VirtualMachineTest, DelayTimerCanBeReadByTheProgram) {
    Write(0, 0x6005); // V[0] = 5
    Write(1, 0xF015); // DT = V[0]
    Write(2, 0xF107); // V[1] = DT
    for (int i = 3; i < 20; i++) {
        Write(i, 0x0001); // NOP
    }
    virtual_machine_set_speed(&vm, 180); // 3 instructions per frame
    EXPECT_EQ(VM_OK, virtual_machine_run_frame(&vm, 0));
    // The timer is set and read in the first frame and decremented once at the end of it
    EXPECT_EQ(5, vm.V[1]);
    EXPECT_EQ(4, vm.delayTimer);
}

} // namespace
