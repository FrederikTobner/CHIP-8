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
 * @file path_utils.h
 * @brief Declarations regarding the path utilities used by the emulator
 */

#ifndef CHIP8_PATH_UTILS_H_
#define CHIP8_PATH_UTILS_H_

#include "pre_compiled_header.h"

/// @brief Determines the path of the emulator executable
/// @param buffer The buffer where the path is stored
/// @param bufferSize The length of the buffer
/// @return 0 if everything went well, -1 if an error occured
int path_utils_get_executable_path(char * buffer, size_t bufferSize);

/// @brief Removes the specified amount of file-layers from the buffer  
/// @param buffer The buffer that is 
/// @param depth The amount of layers that are removed from the path 
void path_utils_remove_file_layer(char * buffer, size_t depth);

/// @brief Adds the specified folder name followed by a os-specific file seperator to the path
/// @param buffer The buffer where the filename is concatenated
/// @param folderName The name of the folder that is concatenated
void path_utils_concatenate_folder(char * buffer, char * folderName);

#endif