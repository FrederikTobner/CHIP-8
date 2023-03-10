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
 * @file pre_compiled_header.h
 * @brief Precompiled header file of the emulator frontend
 * @details Contains commonly used header files from the standard libary files and exit codes
 */

#ifndef CHIP8_PRE_COMPILED_HEADER_FRONTEND_H_
#define CHIP8_PRE_COMPILED_HEADER_FRONTEND_H_

typedef enum {
    EXIT_CODE_ASSEMBLER_ERROR = 65
} chip8_frontend_exit_code;

// Standard libary dependencies
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#endif
