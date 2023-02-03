#ifndef CHIP8_LEXER_H
#define CHIP8_LEXER_H

#include <stdint.h>

// Type definition of a lexer/scanner
typedef struct
{
    // Pointer to the start of the current line where the lexical analysis is performed
    const char * start;
    // Pointer to the current position in the current line where the lexical analysis is performed
    const char * current;
    // Line counter - used for error reporting
    uint32_t line;
} lexer_t;

void lexer_initialize(lexer_t * lexer, char const * source);

int32_t lexer_scan_opcode(lexer_t * lexer);

#endif
