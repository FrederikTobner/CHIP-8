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
 * @file file_utils.c
 * @brief Definitions regarding the file utilities used by the emulator
 */

#include "file_utils.h"

#include "../base/chip8.h"
#include "../base/exit_code.h"

static void io_error(char const *, ...);

char * file_utils_read_file(char const * path) {
    // Opens a file of a nonspecified format (b) in read mode (r)
    FILE * file = fopen(path, "rb");
    if (!file) {
        io_error("Could not open file \"%s\".\n", path);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char * buffer = (char *)malloc(fileSize + 1);
    if (!buffer) {
        io_error("Could not allocate memory to read \"%s\".\n", path);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        io_error("Could not fully read file \"%s\".\n", path);
    }
    // We add the null-character the end of the source-code to mark the end of the file
    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}

void file_utils_read_file_to_memory(char const * path, uint8_t * memory) {
    // Opens a file of a nonspecified format (b) in read mode (r)
    FILE * file = fopen(path, "rb");
    if (!file) {
        io_error("Could not open file \"%s\".\n", path);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    if (fileSize > (4096 - PROGRAM_START_LOCATION)) {
        io_error("Content is to big to store it in memory \"%s\".\n", path);
    }
    size_t bytesRead = fread(memory + PROGRAM_START_LOCATION, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        io_error("Could not fully read file \"%s\".\n", path);
    }
}

/// @brief Reports an error that has occured during a IO operation
/// @param format The format of the error message
/// @param args var-args Used for the error message
static void io_error(char const * format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    exit(EXIT_CODE_INPUT_OUTPUT_ERROR);
}
