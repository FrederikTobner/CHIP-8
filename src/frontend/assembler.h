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
 * @file assembler.h
 * @brief Declarations regarding the assembler of the emulator
 */

#ifndef CHIP8_ASSEMLER_H_
#define CHIP8_ASSEMLER_H_

#include "../backend/chip8.h"

#include "address_hash_table.h"
#include "pre_compiled_header.h"

/// @brief Type definition of a assembler
typedef struct {
    /// Source file
    char * source;
    /// Pointer to the start of the current line
    char const * start;
    /// Pointer to the current position in the current line
    char const * current;
    /// Line counter - used for error reporting
    uint32_t line;
    address_hash_table_t table;
} assembler_t;

/// @brief Initializes the assembler
/// @param assembler The assembler that parses the file
/// @param source The sourcecode that is parsed by the assembler
int assembler_initialize(assembler_t * assembler, char const * source);

/// @brief Processes a chip8 assembly file (.cp8)
/// @param assembler The assembler that processes the file
/// @param chip8 The virtual machine where the program is written into memory
/// @return 0 if everything went well, -1 if an error occured
int assembler_process_file(assembler_t * assembler, chip8_t * chip8);

#endif
