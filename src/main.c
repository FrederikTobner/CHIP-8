#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chip8.h"
#include "common.h"
#include "lexer.h"

static char * read_file(char const * path);

int main(int argc, char const ** argv)
{
    if (argc == 2)
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
    else
    {
        fprintf(stderr, "Usage: Chip8 [path]\n");
        exit(EXIT_CODE_COMMAND_LINE_USAGE_ERROR);
    }
    return EXIT_CODE_OK;
}

static void io_error(char const * format, ...)
{
     va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);
    exit(EXIT_CODE_INPUT_OUTPUT_ERROR);
}

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