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

#ifndef CHIP8_ASSEMLER_H_
#define CHIP8_ASSEMLER_H_

#include "pre_compiled_header.h"

/// @brief Type definition of a assembler
typedef struct {
    /// Pointer to the start of the current line
    const char * start;
    /// Pointer to the current position in the current line
    const char * current;
    /// Line counter - used for error reporting
    uint32_t line;
} assembler_t;

void assembler_initialize(assembler_t * assembler, char const * source);

int32_t assembler_scan_opcode(assembler_t * assembler);

#endif
