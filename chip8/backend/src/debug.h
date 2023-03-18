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
 * @file debug.h
 * @brief Declarations regarding the debug functionality of the emulator
 */

#ifndef CHIP8_DEBUG_H_
#define CHIP8_DEBUG_H_

#include "pre_compiled_header.h"

#include "virtual_machine.h"

/// @brief Prints the opcode stored at the specified memorylocation
/// @param memoryLocation The memory location of the opcode
/// @param opcode The opcode that is printed
void debug_print_bytecode(uint16_t memoryLocation, uint16_t opcode);

/// @brief Traces the exection of a chip8 program
/// @param chip8 The virtual machine where the execution is traced
void debug_trace_execution(virtual_machine_t chip8);

#endif
