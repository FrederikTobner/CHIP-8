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
 * @file debug_bytecode.h
 * @brief Debug output of the bytecode that is stored in the memory of the virtual machine
 */

#ifndef CHIP8_DEBUG_BYTECODE_H_
#define CHIP8_DEBUG_BYTECODE_H_

#include <stdint.h>
#include <stdio.h>

/// @brief Prints the opcode stored at the specified memory location
/// @details Used by the assembler and the virtual machine when the bytecode is printed (CP8_DEBUG_PRINT_BYTECODE)
/// @param memoryLocation The memory location of the opcode
/// @param opcode The opcode that is printed
static inline void debug_print_bytecode(uint16_t memoryLocation, uint16_t opcode) {
    printf("0x%04X: [0x%04X]\n", memoryLocation, opcode);
}

#endif
