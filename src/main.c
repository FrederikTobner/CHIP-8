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
 * @file main.c
 * @brief Main entry point of the program
 */

#include "../build/src/chip8_config.h"
#include "assembler.h"
#include "chip8.h"
#include "display.h"
#include "file_utils.h"
#include "pre_compiled_header.h"

/// Short message that explains the usage of the CHIP-8 emulator
#define CHIP8_USAGE_MESSAGE "Usage: Chip8 [path]\n"

static void show_help();

/// @brief Main entry point of the CHIP-8 program
/// @param argc The amount of arguments that were used when the program was started
/// @param argv Pointer to the arguments array that contains all the arguments that were defined by the user when the
/// program was started
/// @return 0 if everything went well
int main(int argc, char ** args) {
    if (argc == 2) {
        if ((strlen(args[1]) == 7 && !strncmp(args[1], "--version", 7)) ||
            (strlen(args[1]) == 2 && !strncmp(args[1], "-v", 2))) {
            printf("%s Version %i.%i\n", PROJECT_NAME, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
        } else if ((strlen(args[1]) == 6 && !strncmp(args[1], "--help", 6)) ||
                   (strlen(args[1]) == 2 && !strncmp(args[1], "-h", 2))) {
            show_help();
        } else {
            char * source;
            assembler_t assembler;
            chip8_t chip8;
            source = file_utils_read_file(args[1]);
            assembler_initialize(&assembler, source);
            chip8_init(&chip8);
            if (assembler_process_file(&assembler, &chip8)) {
                exit(EXIT_CODE_ASSEMBLER_ERROR);
            }
            // Initialzes the SDL subsystem
            if (display_init(&chip8.display)) {
                exit(EXIT_CODE_SYSTEM_ERROR);
            }
            chip8_execute(&chip8);
            display_quit(&chip8.display);
        }
    } else {
        fprintf(stderr, CHIP8_USAGE_MESSAGE);
        exit(EXIT_CODE_COMMAND_LINE_USAGE_ERROR);
    }
    return EXIT_CODE_OK;
}

/// @brief Displays the help of the emulator
static void show_help() {
    printf("%s Help\n%s\n\n", PROJECT_NAME, CHIP8_USAGE_MESSAGE);
    printf("Options\n");
    printf("  -h, --help\t\tDisplay this help and exit\n");
    printf("  -v, --version\t\tShows the version of the installed emulator and "
           "exit\n\n");
}
