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
#include "backend/chip8.h"
#include "backend/display.h"
#include "frontend/assembler.h"
#include "pre_compiled_header.h"
#include "utils/file_utils.h"


/// Short message that explains the usage of the CHIP-8 emulator
#define CHIP8_USAGE_MESSAGE "Usage: Chip8 [path]\n"
#define PROJECT_INIT_LETTERING \
    ("   _____ _    _ _____ _____        ___  \n\
  / ____| |  | |_   _|  __ \\      / _ \\ \n\
 | |    | |__| | | | | |__) |____| (_) |\n\
 | |    |  __  | | | |  ___/______> _ < \n\
 | |____| |  | |_| |_| |         | (_) |\n\
  \\_____|_|  |_|_____|_|          \\___/ \n\
")

static void run_from_file(char *);
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
            printf("%s Version %i.%i.%i\n", PROJECT_NAME, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR,
                   PROJECT_VERSION_PATCH);
        } else if ((strlen(args[1]) == 6 && !strncmp(args[1], "--help", 6)) ||
                   (strlen(args[1]) == 2 && !strncmp(args[1], "-h", 2))) {
            show_help();
        } else {
            run_from_file(args[1]);
        }
    } else {
        fprintf(stderr, CHIP8_USAGE_MESSAGE);
        exit(EXIT_CODE_COMMAND_LINE_USAGE_ERROR);
    }
    return EXIT_CODE_OK;
}

/// @brief Executes a chip8 program stored in a file
/// @param filePath The path of the program
static void run_from_file(char * filePath) {
    char * source;
    chip8_t chip8;
    size_t pathLength = strlen(filePath);
    chip8_init(&chip8);
    if (pathLength > 4 && !strcmp(filePath + pathLength - 4, ".cp8")) {
        // The source is provided in assembly language
        assembler_t assembler;
        source = file_utils_read_file(filePath);
        assembler_initialize(&assembler, source);
        if (assembler_process_file(&assembler, &chip8)) {
            exit(EXIT_CODE_ASSEMBLER_ERROR);
        }
    } else if (pathLength > 4 && !strcmp(filePath + pathLength - 4, ".ch8")) {
        // The source is provided in binary -> just store it in memory
        file_utils_read_file_to_memory(filePath, &chip8);
    } else {
        fprintf(stderr, "File type not supported");
        exit(EXIT_CODE_COMMAND_LINE_USAGE_ERROR);
    }
    // Initialzes the SDL subsystem
    if (display_init(&chip8.display)) {
        exit(EXIT_CODE_SYSTEM_ERROR);
    }
    printf("%s\t\t\t\t Version %i.%i.%i\n", PROJECT_INIT_LETTERING, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR,
           PROJECT_VERSION_PATCH);
    chip8_execute(&chip8);
    display_quit(&chip8.display);
}

/// @brief Displays the help of the emulator
static void show_help() {
    printf("%s Help\n%s\n\n", PROJECT_NAME, CHIP8_USAGE_MESSAGE);
    printf("Options\n");
    printf("  -h, --help\t\tDisplay this help and exit\n");
    printf("  -v, --version\t\tShows the version of the installed emulator and exit\n\n");
}
