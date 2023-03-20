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
 * @file exit_code.c
 * @brief Exit codes used by the emulator
 */

#ifndef CHIP8_BASE_H_
#define CHIP8_BASE_H_

/// @brief Exit codes of the emulator
typedef enum {
    /// Indicates a sucessfull programm execution
    EXIT_CODE_OK = 0,
    /// Used to indicate a command line usage error
    EXIT_CODE_COMMAND_LINE_USAGE_ERROR = 64,
    /// Dedicated error code of the asembler execution unit
    EXIT_CODE_ASSEMBLER_ERROR = 65,
    /// An error at runtime
    EXIT_CODE_RUNTIME_ERROR = 70,
    /// An error produced by the underlying Operating system
    EXIT_CODE_SYSTEM_ERROR = 71,
    /// Used to indicate an input / output error
    EXIT_CODE_INPUT_OUTPUT_ERROR = 74
} chip8_exit_code;

#endif
