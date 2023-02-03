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

#ifndef CHIP8_LEXER_H
#define CHIP8_LEXER_H

#include <stdint.h>

/// Type definition of a lexer/scanner
typedef struct
{
    // Pointer to the start of the current line where the lexical analysis is performed
    const char * start;
    // Pointer to the current position in the current line where the lexical analysis is performed
    const char * current;
    // Line counter - used for error reporting
    uint32_t line;
} lexer_t;

void lexer_initialize(lexer_t * lexer, char const * source);

int32_t lexer_scan_opcode(lexer_t * lexer);

#endif
