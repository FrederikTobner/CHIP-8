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

#include "../../../external/SDL/include/SDL.h"
#include "backend_pre_compiled_header.h"

#include "display.h"

/// @brief Models a chip8 emulator
typedef struct {
    /// The opcode that is currently executed
    uint16_t currentOpcode;
    /// 16-bit register used for storing an adress in memory
    uint16_t I;
    /// Programcounter of the virtual machine
    uint16_t programCounter;
    /// Delay timer of the virtual machine
    uint8_t delayTimer;
    /// Sound timer of the virtual machine
    uint8_t soundTimer;
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

void virtual_machine_execute(virtual_machine_t * vm);

void virtual_machine_init(virtual_machine_t * vm);

void virtual_machine_write_opcode_to_memory(virtual_machine_t * vm, uint16_t * memoryLocation, uint16_t opcode);

void virtual_machine_write_byte_to_memory(virtual_machine_t * vm, uint16_t * memoryLocation, uint8_t byte);

#endif
