/****************************************************************************
 * Copyright (C) 2022 by Frederik Tobner                                    *
 *                                                                          *
 * This file is part of CHIP8.                                             *
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

#ifndef CHIP8_CHIP8_H_
#define CHIP8_CHIP8_H_

#include <stdint.h>

/// @brief Models a chip8 emulator
typedef struct
{
    // Memory of the virtual machine (4096 bytes)
    uint8_t memory[4096];
    // Registers of the virtual macine (16 8-bit registers)
    uint8_t V[16];
    // Offset of the opcode that is currently processed
    uint16_t currentOpcode;
    uint16_t I;
    uint16_t programCounter;
    // Display of the virtual machine
    uint8_t graphicsSystem[64][32];
    // Delay timer of the virtual machine
    uint8_t delay_timer;
    // Sound timer of the virtual machine
    uint8_t sound_timer;
    // Stack of the chip8 (16bit unsigned integer values)
    uint16_t stack[16];
    // Stackpointer 
    uint16_t * stackPointer;
    uint8_t key[16];
} chip8_t;

void chip8_execute(chip8_t * chip8);

void chip8_init(chip8_t * chip8);

void chip8_write_opcode_to_memory(chip8_t * chip8, uint16_t * memoryLocation, uint16_t opcode);

#endif
