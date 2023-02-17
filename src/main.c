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


#include "../external/SDL/include/SDL.h"

#include "assembler.h"
#include "chip8.h"
#include "../build/src/chip8_config.h"
#include "pre_compiled_header.h"

/// Short message that explains the usage of the CHIP-8 emulator
#define CHIP8_USAGE_MESSAGE     \
"Usage: Chip8 [path]\n"

static char * read_file(char const *);
static void io_error(char const *, ...);
static void show_help();
static void sdl_init(SDL_Window **, SDL_Renderer **);
static void sdl_quit(SDL_Window **, SDL_Renderer **);

/// @brief Main entry point of the CHIP-8 program
/// @param argc The amount of arguments that were used when the program was started
/// @param argv Pointer to the arguments array that contains all the arguments that were defined by the user when the program was started
/// @return 0 if everything went well
int main(int argc, char **args)
{    
    SDL_Window * window = NULL;
    SDL_Renderer * renderer = NULL;
    if (argc == 2)
    {
        if((strlen(args[1]) == 7 && !strncmp(args[1], "--version", 7)) || (strlen(args[1]) == 2 && !strncmp(args[1], "-v", 2)))
            printf("%s Version %i.%i\n", PROJECT_NAME, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
        else if((strlen(args[1]) == 6 && !strncmp(args[1], "--help", 6)) || (strlen(args[1]) == 2 && !strncmp(args[1], "-h", 2)))
            show_help();
        else
        {
            char * source = read_file(args[1]);
            assembler_t lexer;
            chip8_t chip8;
            assembler_initialize(&lexer, source);
            chip8_init(&chip8);
            int32_t opcode;
            uint16_t memoryLocation = 512;
            #ifdef PRINT_BYTE_CODE
                printf("=== Code ===\n");
            #endif
            // Writes all the parsed opcodes into memory
            while((opcode = assembler_scan_opcode(&lexer)) >= 0 && memoryLocation <= 4096)
                chip8_write_opcode_to_memory(&chip8, &memoryLocation, opcode);
            // Initialzes the SDL subsystem
            sdl_init(&window, &renderer);
            chip8_execute(&chip8, renderer);
            free(source);
            sdl_quit(&window, &renderer);
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

/// @brief Displays the help of the emulator
static void show_help()
{
    printf("%s Help\n%s\n\n", PROJECT_NAME, CHIP8_USAGE_MESSAGE);
    printf("Options\n");
    printf("  -h, --help\t\tDisplay this help and exit\n");
    printf("  -v, --version\t\tShows the version of the installed emulator and exit\n\n");
}

static void sdl_init(SDL_Window ** window, SDL_Renderer ** renderer)
{
    // Initialization flag
    bool success = true;
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        // Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
            printf("Warning: Linear texture filtering not enabled!");

        // Create window
        *window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 320, SDL_WINDOW_SHOWN);
        if (*window == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            // Create renderer for window
            *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
            if (*renderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else                
                SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF); // Initialize renderer color to white
        }
    }
}

static void sdl_quit(SDL_Window ** window, SDL_Renderer ** renderer)
{
    // Destroy window
    SDL_DestroyRenderer(*renderer);
    SDL_DestroyWindow(*window);
    *window = NULL;
    *renderer = NULL;
    SDL_Quit();
}
