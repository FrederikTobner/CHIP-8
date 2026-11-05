/****************************************************************************
 * Copyright (C) 2023 by Frederik Tobner                                    *
 *                                                                          *
 * This file is part of CHIP-8.                                             *
 *                                                                          *
 * Permission to use, copy, modify, and distribute this software and its    *
 * documentation under the terms of the GNU General Public License is       *
 * hereby granted.                                                          *
 * No representations are made about the suitability of this software for   *
 * any purpose.                                                             *
 * It is provided "as is" without express or implied warranty.              *
 * See the <https://www.gnu.org/licenses/gpl-3.0.html/>GNU General Public   *
 * License for more details.                                                *
 ****************************************************************************/

/**
 * @file virtual_machine.h
 * @brief Declarations regarding the virtual machine of the emulator
 */

#ifndef CHIP8_VIRTUAL_MACHINE_H_
#define CHIP8_VIRTUAL_MACHINE_H_

// This file is included in the test-suite that is written in c++ using the google-test framework
#include "backend_pre_compiled_header.h"
#include <SDL.h>

#include "display.h"
#include "keyboard_state.h"

/// The default speed of the CHIP-8 CPU in instructions per second
#define VM_DEFAULT_INSTRUCTIONS_PER_SECOND (600u)

/// The frequency of the timers and of the display (in Hz)
#define VM_FRAMES_PER_SECOND               (60u)

/// @brief The result of executing instructions on the virtual machine
typedef enum {
    /// Everything went well, the program can continue
    VM_OK = 0,
    /// The program is finished (EXT opcode, end of the program or end of the memory was reached)
    VM_HALT = 1,
    /// An unknown opcode was encountered
    VM_ERROR = -1
} vm_status_t;

/// @brief Models a chip8 emulator
typedef struct {
    /// The opcode that is currently executed
    uint16_t currentOpcode;
    /// 16-bit register used for storing an adress in memory
    uint16_t I;
    /// Programcounter of the virtual machine (the address of the next instruction, instructions can start at odd
    /// addresses)
    uint16_t programCounter;
    /// Delay timer of the virtual machine
    uint8_t delayTimer;
    /// Sound timer of the virtual machine
    uint8_t soundTimer;
    /// Amount of instructions that are executed between two ticks of the timers (one frame)
    uint16_t instructionsPerFrame;
    /// Determines if the machine is currently waiting for a key press (FX0A)
    bool waitingForKey;
    /// The keys that were already pressed (and not released) when the machine started to wait for a key press
    keyBoardState_t keyWaitBaseline;
    /// Stackpointer
    uint16_t * stackPointer;
    /// Display of the emulator
    display_t display;
    /// Stack of the chip8 (16bit unsigned integer values)
    uint16_t stack[16];
    /// Memory of the virtual machine (4096 bytes)
    uint8_t memory[4096];
    /// Registers of the virtual macine (16 8-bit registers)
    uint8_t V[16];
} virtual_machine_t;

/// @brief Executes the program that is stored in memory until it is finished or the window is closed
/// @param vm The chip8 vm where the program that is currently held in memory is executed
void virtual_machine_execute(virtual_machine_t * vm);

/// @brief Initializes the chip8 vm
/// @param vm The chip8 virtual machine that is initialzed
void virtual_machine_init(virtual_machine_t * vm);

/// @brief Sets the speed of the virtual machine
/// @param vm The chip8 virtual machine where the speed is set
/// @param instructionsPerSecond The amount of instructions that are executed per second (rounded to a whole amount of
/// instructions per frame, at least one instruction per frame)
void virtual_machine_set_speed(virtual_machine_t * vm, uint32_t instructionsPerSecond);

/// @brief Executes the next instruction of the program
/// @param vm The chip8 virtual machine that executes the instruction
/// @param keyBoardState The keys that are currently pressed
/// @return VM_OK if the program can continue, VM_HALT if it is finished and VM_ERROR if the opcode is unknown
vm_status_t virtual_machine_step(virtual_machine_t * vm, keyBoardState_t keyBoardState);

/// @brief Executes the instructions of a single frame (1/60 second) and ticks the timers afterwards
/// @param vm The chip8 virtual machine that executes the instructions
/// @param keyBoardState The keys that are currently pressed
/// @return VM_OK if the program can continue, VM_HALT if it is finished and VM_ERROR if the opcode is unknown
vm_status_t virtual_machine_run_frame(virtual_machine_t * vm, keyBoardState_t keyBoardState);

void virtual_machine_write_opcode_to_memory(virtual_machine_t * vm, uint16_t * memoryLocation, uint16_t opcode);

void virtual_machine_write_byte_to_memory(virtual_machine_t * vm, uint16_t * memoryLocation, uint8_t byte);

#endif
