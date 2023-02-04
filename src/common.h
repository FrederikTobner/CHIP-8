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

#ifndef CHIP8_COMMON_H_
#define CHIP8_COMMON_H_
typedef enum
{
    EXIT_CODE_OK = 0,
    EXIT_CODE_COMMAND_LINE_USAGE_ERROR = 64,
    EXIT_CODE_COMPILATION_ERROR = 65,
    EXIT_CODE_RUNTIME_ERROR = 70,
    EXIT_CODE_SYSTEM_ERROR = 71,
    EXIT_CODE_INPUT_OUTPUT_ERROR = 74
} chip8_exit_code;

#endif
