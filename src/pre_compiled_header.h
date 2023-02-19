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
 * @brief Precompiled header file of the emulator
 * @details Contains commonly used header files from the standard libary and exit codes
 */

#ifndef CHIP8_PRE_COMPILED_HEADER_H_
#define CHIP8_PRE_COMPILED_HEADER_H_
typedef enum {
    EXIT_CODE_OK = 0,
    EXIT_CODE_COMMAND_LINE_USAGE_ERROR = 64,
    EXIT_CODE_ASSEMBLER_ERROR = 65,
    EXIT_CODE_RUNTIME_ERROR = 70,
    EXIT_CODE_SYSTEM_ERROR = 71,
    EXIT_CODE_INPUT_OUTPUT_ERROR = 74
} chip8_exit_code;

// Standard libary dependencies
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(OS_WINDOWS)
#include <conio.h>
#include <windows.h>
#include <psapi.h>
#elif defined(OS_UNIX_LIKE)
#include <curses.h>
#include <unistd.h>
#endif

#endif
