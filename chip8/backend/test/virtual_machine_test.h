#ifndef CHIP8_VIRTUAL_MACHINE_TEST_H_
#define CHIP8_VIRTUAL_MACHINE_TEST_H_

#include <gtest/gtest.h>

#include <stdint.h>
#include <string.h>

#include "../src/virtual_machine.h"

constexpr uint16_t kProgramStart = 0x200;

/// Fixture that provides a virtual machine without any window - the SDL video subsystem is never initialized
class VirtualMachineTest : public ::testing::Test {
  protected:
    void SetUp() override {
        // Filling the struct with garbage makes sure that the initialization does not rely on zeroed memory
        memset(&vm, 0xA5, sizeof(vm));
        virtual_machine_init(&vm);
    }

    /// The index of the instruction that is executed next, relative to the start of the program
    int Instruction() const {
        return (vm.programCounter - kProgramStart) / 2;
    }

    /// Writes the opcode as the instruction with the specified index of the program
    void Write(uint16_t index, uint16_t opcode) {
        vm.memory[kProgramStart + index * 2] = opcode >> 8;
        vm.memory[kProgramStart + index * 2 + 1] = opcode & 0xFF;
    }

    /// Writes the opcode as the first instruction of the program and executes it
    vm_status_t Execute(uint16_t opcode, keyBoardState_t keys = 0) {
        vm.programCounter = kProgramStart;
        Write(0, opcode);
        return virtual_machine_step(&vm, keys);
    }

    /// Counts the pixels that are set in the graphics system
    int LitPixels() const {
        int count = 0;
        for (int x = 0; x < GRAPHICS_SYSTEM_WIDTH; x++) {
            for (int y = 0; y < GRAPHICS_SYSTEM_HEIGHT; y++) {
                count += vm.display.graphicsSystem[x][y] ? 1 : 0;
            }
        }
        return count;
    }

    virtual_machine_t vm;
};

#endif
