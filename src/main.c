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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "chip8_config.h"
#include "common.h"
#include "flags.h"
#include "lexer.h"

/// Short message that explains the usage of the CHIP8 emulator
#define CHIP8_USAGE_MESSAGE     \
"Usage: Chip8 [path]\n"

static char * read_file(char const * path);
static void io_error(char const * format, ...);

/// @brief Main entry point of the CHIP8 program
/// @param argc The amount of arguments that were used when the program was started
/// @param argv Pointer to the arguments array that contains all the arguments that were defined by the user when the program was started
/// @return 0 if everything went well
int main(int argc, char const ** argv)
{
    if (argc == 2)
    {
        if(!strncmp(argv[1], "--version", 7) || !strncmp(argv[1], "-v", 2))
            printf("CHIP8 Version %i.%i\n", PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
        else if(!strncmp(argv[1], "--help", 6) || !strncmp(argv[1], "-h", 2))
            fprintf(stderr, CHIP8_USAGE_MESSAGE);
        else
        {
            char * source = read_file(argv[1]);
            lexer_t lexer;
            chip8_t chip8;
            lexer_initialize(&lexer, source);
            chip8_init(&chip8);
            int32_t opcode;
            uint16_t memoryLocation = 0;
            #ifdef PRINT_BYTE_CODE
                printf("=== Code ===\n");
            #endif
            // Writes all the parsed opcodes into memory
            while((opcode = lexer_scan_opcode(&lexer)) >= 0 && memoryLocation <= 4096)
                chip8_write_opcode_to_memory(&chip8, &memoryLocation, opcode);
            chip8_execute(&chip8);
            free(source);
        }
    }
    else
    {
        fprintf(stderr, CHIP8_USAGE_MESSAGE);
        exit(EXIT_CODE_COMMAND_LINE_USAGE_ERROR);
    }
    return EXIT_CODE_OK;
}

/// @brief Reports an error that has occured during a IO operation
/// @param format The format of the error message
/// @param args var-args Used for the error message
static void io_error(char const * format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    exit(EXIT_CODE_INPUT_OUTPUT_ERROR);
}

/// @brief Reads a file from a specified location and returns the content of the file as a character pointer
/// @param path The path of the file that is read
/// @return The content of the file as a character pointer
static char * read_file(char const * path)
{
    // Opens a file of a nonspecified format (b) in read mode (r)
    FILE * file = fopen(path, "rb");
    if (!file)
        io_error("Could not open file \"%s\".\n", path);
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char * buffer = (char *)malloc(fileSize + 1);
    if (!buffer)
        io_error("Not enough memory to read \"%s\".\n", path);
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
        io_error("Could not read file \"%s\".\n", path);
    // We add null the end of the source-code to mark the end of the file
    buffer[bytesRead] = '\0';
    fclose(file);
    return buffer;
}