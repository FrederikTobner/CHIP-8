#include "virtual_machine_test.h"

namespace {

class OpcodeTest : public VirtualMachineTest {
  protected:
    bool Pixel(int x, int y) const {
        return vm.display.graphicsSystem[x][y] != 0;
    }
};

TEST_F(OpcodeTest, DrawSprite_LeftmostBitIsLeftmostPixel) {
    vm.memory[0x900] = 0x80;
    vm.I = 0x900;
    vm.V[0] = 5;
    vm.V[1] = 3;
    ASSERT_EQ(VM_OK, Execute(0xD011));
    EXPECT_TRUE(Pixel(5, 3));
    EXPECT_FALSE(Pixel(4, 3));
    EXPECT_FALSE(Pixel(6, 3));
    EXPECT_EQ(1, LitPixels());
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, DrawSprite_RightmostBitIsEightPixelsToTheRight) {
    vm.memory[0x900] = 0x01;
    vm.I = 0x900;
    vm.V[0] = 5;
    vm.V[1] = 3;
    ASSERT_EQ(VM_OK, Execute(0xD011));
    EXPECT_TRUE(Pixel(12, 3));
    EXPECT_EQ(1, LitPixels());
}

TEST_F(OpcodeTest, DrawSprite_DrawsAllRowsAndBits) {
    vm.memory[0x900] = 0xFF;
    vm.memory[0x901] = 0xA5;
    vm.I = 0x900;
    vm.V[0] = 10;
    vm.V[1] = 4;
    ASSERT_EQ(VM_OK, Execute(0xD012));
    for (int x = 10; x < 18; x++) {
        EXPECT_TRUE(Pixel(x, 4));
    }
    bool const secondRow[8] = {true, false, true, false, false, true, false, true};
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ(secondRow[i], Pixel(10 + i, 5)) << i;
    }
}

TEST_F(OpcodeTest, DrawSprite_SecondDrawErasesAndSetsCollisionFlag) {
    vm.memory[0x900] = 0xF0;
    vm.I = 0x900;
    ASSERT_EQ(VM_OK, Execute(0xD001));
    EXPECT_EQ(0, vm.V[0xF]);
    EXPECT_EQ(4, LitPixels());
    ASSERT_EQ(VM_OK, Execute(0xD001));
    EXPECT_EQ(1, vm.V[0xF]);
    EXPECT_EQ(0, LitPixels());
}

TEST_F(OpcodeTest, DrawSprite_CollisionFlagIsResetByTheNextDraw) {
    vm.memory[0x900] = 0x80;
    vm.I = 0x900;
    ASSERT_EQ(VM_OK, Execute(0xD001));
    ASSERT_EQ(VM_OK, Execute(0xD001));
    ASSERT_EQ(1, vm.V[0xF]);
    vm.V[0] = 20;
    ASSERT_EQ(VM_OK, Execute(0xD001));
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, DrawSprite_OnlyOverlappingPixelsCollide) {
    vm.memory[0x900] = 0xC0;
    vm.memory[0x901] = 0x80;
    vm.I = 0x900;
    ASSERT_EQ(VM_OK, Execute(0xD001));
    // The pixel of the second sprite only overlaps with the second pixel of the first one
    vm.I = 0x901;
    vm.V[0] = 1;
    vm.V[1] = 0;
    ASSERT_EQ(VM_OK, Execute(0xD011));
    EXPECT_EQ(1, vm.V[0xF]);
    EXPECT_TRUE(Pixel(0, 0));
    EXPECT_FALSE(Pixel(1, 0));
    EXPECT_FALSE(Pixel(2, 0));
}

TEST_F(OpcodeTest, DrawSprite_WrapsAroundTheScreen) {
    vm.memory[0x900] = 0xC0;
    vm.I = 0x900;
    vm.V[0] = 63;
    vm.V[1] = 31;
    ASSERT_EQ(VM_OK, Execute(0xD011));
    EXPECT_TRUE(Pixel(63, 31));
    EXPECT_TRUE(Pixel(0, 31));
    EXPECT_EQ(2, LitPixels());
}

TEST_F(OpcodeTest, DrawSprite_YCoordinateInVF) {
    vm.memory[0x900] = 0x80;
    vm.I = 0x900;
    vm.V[0] = 0;
    vm.V[0xF] = 5;
    // VF is reset by the draw, but the coordinate has to be read before that
    ASSERT_EQ(VM_OK, Execute(0xD0F1));
    EXPECT_TRUE(Pixel(0, 5));
    EXPECT_EQ(1, LitPixels());
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, DrawSprite_XCoordinateInVF) {
    vm.memory[0x900] = 0x80;
    vm.I = 0x900;
    vm.V[1] = 2;
    vm.V[0xF] = 7;
    ASSERT_EQ(VM_OK, Execute(0xDF11));
    EXPECT_TRUE(Pixel(7, 2));
    EXPECT_EQ(1, LitPixels());
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, DrawSprite_BothCoordinatesInVF) {
    vm.memory[0x900] = 0x80;
    vm.I = 0x900;
    vm.V[0xF] = 3;
    ASSERT_EQ(VM_OK, Execute(0xDFF1));
    EXPECT_TRUE(Pixel(3, 3));
    EXPECT_EQ(1, LitPixels());
}

TEST_F(OpcodeTest, DrawSprite_CollisionIsReportedWhenACoordinateIsInVF) {
    vm.memory[0x900] = 0x80;
    vm.I = 0x900;
    vm.V[0] = 0;
    vm.V[0xF] = 5;
    vm.display.graphicsSystem[0][5] = 1;
    ASSERT_EQ(VM_OK, Execute(0xD0F1));
    // The sprite erased the pixel at (0, 5) and VF holds the collision flag afterwards
    EXPECT_FALSE(Pixel(0, 5));
    EXPECT_EQ(0, LitPixels());
    EXPECT_EQ(1, vm.V[0xF]);
}

TEST_F(OpcodeTest, SkipIfRegistersAreNotEqual_Skips) {
    vm.V[0] = 1;
    vm.V[1] = 2;
    ASSERT_EQ(VM_OK, Execute(0x9010));
    EXPECT_EQ(2, Instruction());
}

TEST_F(OpcodeTest, SkipIfRegistersAreNotEqual_DoesNotSkipOnEqualValues) {
    vm.V[0] = 2;
    vm.V[1] = 2;
    ASSERT_EQ(VM_OK, Execute(0x9010));
    EXPECT_EQ(1, Instruction());
}

TEST_F(OpcodeTest, SkipIfRegistersAreEqual_Skips) {
    vm.V[0] = 2;
    vm.V[1] = 2;
    ASSERT_EQ(VM_OK, Execute(0x5010));
    EXPECT_EQ(2, Instruction());
}

TEST_F(OpcodeTest, CallAndReturn) {
    Write(0, 0x2208); // CAL 0x208 (instruction 4)
    Write(1, 0x6107); // V1 = 7
    Write(2, 0x0002); // EXT
    Write(4, 0x6005); // V0 = 5
    Write(5, 0x00EE); // RET
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(4, Instruction());
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(5, vm.V[0]);
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    // Execution continues after the call
    EXPECT_EQ(1, Instruction());
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(7, vm.V[1]);
    EXPECT_EQ(VM_HALT, virtual_machine_step(&vm, 0));
}

TEST_F(OpcodeTest, NestedCalls) {
    Write(0, 0x2204); // CAL 0x204 (instruction 2)
    Write(1, 0x0002); // EXT
    Write(2, 0x2208); // CAL 0x208 (instruction 4)
    Write(3, 0x00EE); // RET
    Write(4, 0x6001); // V0 = 1
    Write(5, 0x00EE); // RET
    for (int i = 0; i < 5; i++) {
        ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0)) << i;
    }
    EXPECT_EQ(1, vm.V[0]);
    EXPECT_EQ(VM_HALT, virtual_machine_step(&vm, 0));
}

TEST_F(OpcodeTest, Call_SixteenNestedCallsAreAllowed) {
    Write(0, 0x2200); // CAL 0x200 (calls itself)
    for (unsigned i = 0; i < VM_STACK_SIZE; i++) {
        ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0)) << i;
    }
    EXPECT_EQ(vm.stack + VM_STACK_SIZE, vm.stackPointer);
}

TEST_F(OpcodeTest, Call_OverflowOfTheStackIsAnError) {
    Write(0, 0x2200); // CAL 0x200 (calls itself)
    for (unsigned i = 0; i < VM_STACK_SIZE; i++) {
        ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0)) << i;
    }
    EXPECT_EQ(VM_ERROR, virtual_machine_step(&vm, 0));
    EXPECT_EQ(vm.stack + VM_STACK_SIZE, vm.stackPointer);
}

TEST_F(OpcodeTest, Call_OverflowOfTheStackDoesNotOverwriteMemory) {
    Write(0, 0x2200); // CAL 0x200 (calls itself)
    uint8_t const fontSprite = vm.memory[0x50];
    ASSERT_NE(0, fontSprite);
    for (unsigned i = 0; i < VM_STACK_SIZE + 1; i++) {
        virtual_machine_step(&vm, 0);
    }
    // The stack is located directly in front of the memory in the virtual machine
    EXPECT_EQ(0, vm.memory[0]);
    EXPECT_EQ(0, vm.memory[1]);
    EXPECT_EQ(fontSprite, vm.memory[0x50]);
}

TEST_F(OpcodeTest, Return_WithAnEmptyStackIsAnError) {
    EXPECT_EQ(VM_ERROR, Execute(0x00EE));
    EXPECT_EQ(vm.stack, vm.stackPointer);
}

TEST_F(OpcodeTest, Return_UnderflowAfterTheLastReturnIsAnError) {
    Write(0, 0x2204); // CAL 0x204 (instruction 2)
    Write(1, 0x00EE); // RET without a call
    Write(2, 0x00EE); // RET
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(1, Instruction());
    EXPECT_EQ(vm.stack, vm.stackPointer);
    EXPECT_EQ(VM_ERROR, virtual_machine_step(&vm, 0));
    EXPECT_EQ(vm.stack, vm.stackPointer);
}

TEST_F(OpcodeTest, JumpWithOffset) {
    vm.V[0] = 4;
    ASSERT_EQ(VM_OK, Execute(0xB204)); // 0x204 + V0 = 0x208 (instruction 4)
    EXPECT_EQ(4, Instruction());
    Write(4, 0x6A09);
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(9, vm.V[0xA]);
}

TEST_F(OpcodeTest, Add_SetsCarry) {
    vm.V[0] = 200;
    vm.V[1] = 100;
    ASSERT_EQ(VM_OK, Execute(0x8014));
    EXPECT_EQ(44, vm.V[0]);
    EXPECT_EQ(1, vm.V[0xF]);
}

TEST_F(OpcodeTest, Add_ClearsCarry) {
    vm.V[0] = 1;
    vm.V[1] = 2;
    vm.V[0xF] = 1;
    ASSERT_EQ(VM_OK, Execute(0x8014));
    EXPECT_EQ(3, vm.V[0]);
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, Add_FlagWinsWhenTheTargetIsVF) {
    vm.V[0xF] = 200;
    vm.V[1] = 100;
    ASSERT_EQ(VM_OK, Execute(0x8F14));
    EXPECT_EQ(1, vm.V[0xF]);
}

TEST_F(OpcodeTest, Subtract_NoBorrow) {
    vm.V[0] = 5;
    vm.V[1] = 3;
    ASSERT_EQ(VM_OK, Execute(0x8015));
    EXPECT_EQ(2, vm.V[0]);
    EXPECT_EQ(1, vm.V[0xF]);
}

TEST_F(OpcodeTest, Subtract_Borrow) {
    vm.V[0] = 3;
    vm.V[1] = 5;
    vm.V[0xF] = 1;
    ASSERT_EQ(VM_OK, Execute(0x8015));
    EXPECT_EQ(254, vm.V[0]);
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, Subtract_EqualValuesDoNotBorrow) {
    vm.V[0] = 5;
    vm.V[1] = 5;
    ASSERT_EQ(VM_OK, Execute(0x8015));
    EXPECT_EQ(0, vm.V[0]);
    EXPECT_EQ(1, vm.V[0xF]);
}

TEST_F(OpcodeTest, ReverseSubtract) {
    vm.V[0] = 3;
    vm.V[1] = 5;
    ASSERT_EQ(VM_OK, Execute(0x8017));
    EXPECT_EQ(2, vm.V[0]);
    EXPECT_EQ(1, vm.V[0xF]);
    vm.V[0] = 5;
    vm.V[1] = 3;
    ASSERT_EQ(VM_OK, Execute(0x8017));
    EXPECT_EQ(254, vm.V[0]);
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, ShiftRight_StoresLeastSignificantBit) {
    vm.V[0] = 0x05;
    ASSERT_EQ(VM_OK, Execute(0x8006));
    EXPECT_EQ(0x02, vm.V[0]);
    EXPECT_EQ(1, vm.V[0xF]);
    ASSERT_EQ(VM_OK, Execute(0x8006));
    EXPECT_EQ(0x01, vm.V[0]);
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, ShiftLeft_StoresMostSignificantBit) {
    vm.V[0] = 0x81;
    ASSERT_EQ(VM_OK, Execute(0x800E));
    EXPECT_EQ(0x02, vm.V[0]);
    EXPECT_EQ(1, vm.V[0xF]);
    ASSERT_EQ(VM_OK, Execute(0x800E));
    EXPECT_EQ(0x04, vm.V[0]);
    EXPECT_EQ(0, vm.V[0xF]);
}

TEST_F(OpcodeTest, LogicalOperations) {
    vm.V[0] = 0xF0;
    vm.V[1] = 0x3C;
    ASSERT_EQ(VM_OK, Execute(0x8011));
    EXPECT_EQ(0xFC, vm.V[0]);
    vm.V[0] = 0xF0;
    ASSERT_EQ(VM_OK, Execute(0x8012));
    EXPECT_EQ(0x30, vm.V[0]);
    vm.V[0] = 0xF0;
    ASSERT_EQ(VM_OK, Execute(0x8013));
    EXPECT_EQ(0xCC, vm.V[0]);
}

TEST_F(OpcodeTest, SetFontCharacterLocation) {
    vm.V[0] = 0xA;
    ASSERT_EQ(VM_OK, Execute(0xF029));
    EXPECT_EQ(0x50 + 5 * 0xA, vm.I);
    vm.V[0] = 0;
    ASSERT_EQ(VM_OK, Execute(0xF029));
    EXPECT_EQ(0x50, vm.I);
    // The sprite of 0 is 0xF0 0x90 0x90 0x90 0xF0
    EXPECT_EQ(0xF0, vm.memory[vm.I]);
    EXPECT_EQ(0x90, vm.memory[vm.I + 1]);
}

TEST_F(OpcodeTest, BinaryCodedDecimal) {
    vm.V[0] = 254;
    vm.I = 0x900;
    ASSERT_EQ(VM_OK, Execute(0xF033));
    EXPECT_EQ(2, vm.memory[0x900]);
    EXPECT_EQ(5, vm.memory[0x901]);
    EXPECT_EQ(4, vm.memory[0x902]);
}

TEST_F(OpcodeTest, StoreAndLoadRegisters) {
    vm.I = 0x900;
    vm.V[0] = 1;
    vm.V[1] = 2;
    vm.V[2] = 3;
    ASSERT_EQ(VM_OK, Execute(0xF155));
    EXPECT_EQ(1, vm.memory[0x900]);
    EXPECT_EQ(2, vm.memory[0x901]);
    EXPECT_EQ(0, vm.memory[0x902]);
    vm.memory[0x902] = 9;
    ASSERT_EQ(VM_OK, Execute(0xF265));
    EXPECT_EQ(9, vm.V[2]);
}

TEST_F(OpcodeTest, Init_ClearsTheDisplay) {
    EXPECT_EQ(0, LitPixels());
    EXPECT_FALSE(vm.display.dirty);
}

TEST_F(OpcodeTest, DisplayIsDirtyAfterDrawingASprite) {
    vm.memory[0x900] = 0x80;
    vm.I = 0x900;
    ASSERT_EQ(VM_OK, Execute(0xD001));
    EXPECT_TRUE(vm.display.dirty);
}

TEST_F(OpcodeTest, DisplayIsDirtyAfterClearingTheScreen) {
    ASSERT_EQ(VM_OK, Execute(0x00E0));
    EXPECT_TRUE(vm.display.dirty);
}

TEST_F(OpcodeTest, DisplayIsDirtyAfterTogglingTheScreen) {
    ASSERT_EQ(VM_OK, Execute(0x00E1));
    EXPECT_TRUE(vm.display.dirty);
    EXPECT_EQ(GRAPHICS_SYSTEM_WIDTH * GRAPHICS_SYSTEM_HEIGHT, LitPixels());
}

TEST_F(OpcodeTest, DisplayIsNotDirtyAfterOtherOpcodes) {
    vm.V[0] = 1;
    uint16_t const opcodes[] = {0x6001, 0x7001, 0x8014, 0xA900, 0x3000, 0x0001};
    for (uint16_t opcode : opcodes) {
        ASSERT_EQ(VM_OK, Execute(opcode)) << opcode;
        EXPECT_FALSE(vm.display.dirty) << opcode;
    }
}

TEST_F(OpcodeTest, DisplayStaysDirtyUntilItIsRendered) {
    vm.memory[0x900] = 0x80;
    vm.I = 0x900;
    ASSERT_EQ(VM_OK, Execute(0xD001));
    for (int i = 0; i < 40; i++) {
        Write(i + 1, 0x0001); // NOP
    }
    ASSERT_EQ(VM_OK, virtual_machine_run_frame(&vm, 0));
    EXPECT_TRUE(vm.display.dirty);
}

class KeyOpcodeTest : public OpcodeTest, public ::testing::WithParamInterface<int> {};

TEST_P(KeyOpcodeTest, SkipIfKeyIsPressed) {
    int const key = GetParam();
    vm.V[2] = key;
    ASSERT_EQ(VM_OK, Execute(0xE29E, 1u << key));
    EXPECT_EQ(2, Instruction());
    // Every other key does not skip
    ASSERT_EQ(VM_OK, Execute(0xE29E, (~(1u << key)) & 0xFFFF));
    EXPECT_EQ(1, Instruction());
}

TEST_P(KeyOpcodeTest, SkipIfKeyIsNotPressed) {
    int const key = GetParam();
    vm.V[2] = key;
    ASSERT_EQ(VM_OK, Execute(0xE2A1, 1u << key));
    EXPECT_EQ(1, Instruction());
    ASSERT_EQ(VM_OK, Execute(0xE2A1, (~(1u << key)) & 0xFFFF));
    EXPECT_EQ(2, Instruction());
}

INSTANTIATE_TEST_SUITE_P(AllKeys, KeyOpcodeTest, ::testing::Range(0, 16));

TEST_F(OpcodeTest, KeyOpcodes_OnlyUseTheLowerNibbleOfTheRegister) {
    vm.V[2] = 0x13; // key 3
    ASSERT_EQ(VM_OK, Execute(0xE29E, 1u << 3));
    EXPECT_EQ(2, Instruction());
}

TEST_F(OpcodeTest, WaitForKey_BlocksUntilAKeyIsPressed) {
    Write(0, 0xF50A); // V5 = wait for key
    Write(1, 0x6101); // V1 = 1
    for (int i = 0; i < 3; i++) {
        ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
        EXPECT_EQ(0, Instruction());
    }
    EXPECT_EQ(0, vm.V[1]);
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 1u << 7));
    EXPECT_EQ(7, vm.V[5]);
    EXPECT_EQ(1, Instruction());
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(1, vm.V[1]);
}

TEST_F(OpcodeTest, WaitForKey_IgnoresKeysThatAreAlreadyHeld) {
    Write(0, 0xF50A);
    // The key was pressed before the machine started to wait
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 1u << 4));
    EXPECT_EQ(0, Instruction());
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 1u << 4));
    EXPECT_EQ(0, Instruction());
    // Release and press the key again
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    EXPECT_EQ(0, Instruction());
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 1u << 4));
    EXPECT_EQ(1, Instruction());
    EXPECT_EQ(4, vm.V[5]);
}

TEST_F(OpcodeTest, WaitForKey_NewKeyWhileAnotherIsHeld) {
    Write(0, 0xF50A);
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 1u << 4));
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, (1u << 4) | (1u << 9)));
    EXPECT_EQ(1, Instruction());
    EXPECT_EQ(9, vm.V[5]);
}

TEST_F(OpcodeTest, WaitForKey_CanBeUsedTwice) {
    Write(0, 0xF50A);
    Write(1, 0xF60A);
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 1u << 1));
    EXPECT_EQ(1, vm.V[5]);
    // The key is still held down when the next wait starts
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 1u << 1));
    EXPECT_EQ(1, Instruction());
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 0));
    ASSERT_EQ(VM_OK, virtual_machine_step(&vm, 1u << 2));
    EXPECT_EQ(2, vm.V[6]);
    EXPECT_EQ(2, Instruction());
}

} // namespace
