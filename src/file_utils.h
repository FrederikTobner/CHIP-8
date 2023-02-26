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
 * @file file_utils.h
 * @brief Declarations regarding the file utilities used by the emulator
 */

#ifndef CHIP8_FILE_UTILS_H_
#define CHIP8_FILE_UTILS_H_

#include "chip8.h"

/// @brief Reads a file from a specified location and returns the content of the file as a character pointer
/// @param path The path of the file that is read
/// @return The content of the file as a character pointer
char * file_utils_read_file(char const *);

void file_utils_read_file_to_memory(char const * path, chip8_t * chip8);

#endif
