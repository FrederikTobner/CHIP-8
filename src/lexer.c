/****************************************************************************
 * Copyright (C) 2022 by Frederik Tobner                                    *
 *                                                                          *
 * This file is part of CHIP8.                                             *
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

#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

static inline char lexer_advance(lexer_t * lexer);
static uint16_t lexer_hexa(lexer_t * lexer, size_t digitCount);
static inline bool lexer_is_alpha(char c);
static inline bool lexer_is_at_end(lexer_t lexer);
static inline bool lexer_is_decimal(char c);
static inline bool lexer_is_hexa(char c);
static inline char lexer_peek(lexer_t lexer);
static uint8_t lexer_read_8bit_number(lexer_t * lexer);
static uint16_t lexer_read_12bit_number(lexer_t * lexer);
static uint8_t lexer_read_register(lexer_t * lexer);
static uint8_t lexer_read_registers(lexer_t * lexer);
static inline void lexer_report_error(lexer_t lexer);
static uint16_t lexer_scan_mnemonic(lexer_t * lexer, char c);
static void lexer_skip_whitespace(lexer_t * lexer);

/// @brief Initialzes the lexer that is used to group the characters in the sourcefile to Tokens
/// @param lexer The lexer that is inialized
/// @param source The content of the source file that is processed by the lexer
void lexer_initialize(lexer_t * lexer, char const * source)
{
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1u;
}

/// @brief Scans the next opcode in the sourcefile
/// @param lexer The lexer where the next opcode is scanned
/// @return The opcode or -1 if we have reached the end of the file or the opcode wasn't processed properly
int32_t lexer_scan_opcode(lexer_t * lexer)
{
    lexer_skip_whitespace(lexer);
    if (lexer_is_at_end(*lexer))
        return -1;
    lexer->start = lexer->current;
    char c = lexer_advance(lexer);
    if(lexer_is_alpha(c))
        return lexer_scan_mnemonic(lexer, c);
    if (c == '0' && (lexer_peek(*lexer) == 'x' || lexer_peek(*lexer) == 'X'))
        return lexer_hexa(lexer, 4);
    return -1;
}

/// @brief Advances a position further in the sourceCode and returns the prevoius Token
/// @param lexer Advances a position further in the sourcecode
/// @return The current character
static inline char lexer_advance(lexer_t * lexer)
{
    return *lexer->current++;
}
    
static uint16_t lexer_hexa(lexer_t * lexer, size_t digitCount)
{
    lexer->current++;
    uint16_t number = 0;
    for (size_t i = 0; i < digitCount; i++)
    {
        number *= 16;
    if(!lexer_is_hexa(lexer_peek(*lexer)))
        lexer_report_error(*lexer);
    if(lexer_is_decimal(lexer_peek(*lexer)))
        number += *lexer->current++ - '0';
    else if(lexer_peek(*lexer) <= 'F')
        number += *lexer->current++ - 'A' + 10;
    else
        number += *lexer->current++ - 'a' + 10;
    }
    return number;    
}

// checks if the char c is a from the alphabet
static inline bool lexer_is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Determines wheather we reached the end in the sourceCode
static inline bool lexer_is_at_end(lexer_t lexer)
{
    return *lexer.current == '\0';
}

// checks if the char c is a digit
static inline bool lexer_is_decimal(char c)
{
    return c >= '0' && c <= '9';
}

// checks if the char c is a hexadecimal digit
static inline bool lexer_is_hexa(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline char lexer_peek(lexer_t lexer)
{
    return *lexer.current;
}

static uint8_t lexer_read_8bit_number(lexer_t * lexer)
{
    lexer_skip_whitespace(lexer);
    char c = lexer_advance(lexer);
    if (c == '0' && (lexer_peek(*lexer) == 'x' || lexer_peek(*lexer) == 'X'))
        return lexer_hexa(lexer, 2);
    lexer_report_error(*lexer);
}

static uint16_t lexer_read_12bit_number(lexer_t * lexer)
{
    lexer_skip_whitespace(lexer);
    char c = lexer_advance(lexer);
    if (c == '0' && (lexer_peek(*lexer) == 'x' || lexer_peek(*lexer) == 'X'))
        return lexer_hexa(lexer, 3);
    lexer_report_error(*lexer);
}

static uint8_t lexer_read_register(lexer_t * lexer)
{
    lexer_skip_whitespace(lexer);
    switch (lexer_advance(lexer))
    {
        case 'V':
            if(!lexer_is_hexa(lexer_peek(*lexer)))
                lexer_report_error(*lexer);
            lexer->current--;
            return lexer_hexa(lexer, 1);
        default:
            lexer_report_error(*lexer);
            break;
    }
}

static uint8_t lexer_read_registers(lexer_t * lexer)
{
    uint8_t value = lexer_read_register(lexer);
    value <<= 4;
    
    return value += lexer_read_register(lexer);
}

static inline void lexer_report_error(lexer_t lexer)
{
    printf("Unexpected character %c in opcode sequence in line %i", *lexer.current, lexer.line);
    exit(EXIT_CODE_COMPILATION_ERROR);
}

static uint16_t lexer_scan_mnemonic(lexer_t * lexer, char c)
{
    switch (c)
    {        
    case 'A':
        switch (lexer_advance(lexer))
            {
            case 'D':
                switch (lexer_advance(lexer))
                {
                case 'D':
                    lexer_skip_whitespace(lexer);
                    switch (lexer_peek(*lexer))
                    {
                    case 'I':
                        lexer_advance(lexer);
                        return 0xF01E | lexer_read_register(lexer) << 8;
                    case 'V':
                        return 0x8004 |lexer_read_registers(lexer) << 4;                    
                    default:
                        lexer_report_error(*lexer);
                    }
                default:
                    lexer_report_error(*lexer);
                }
            default:
                lexer_report_error(*lexer);
            }
    case 'C':
        switch (lexer_advance(lexer))
        {
        case 'A':
            switch (lexer_advance(lexer))
            {
            case 'L':
                return 0x2000 | lexer_read_12bit_number(lexer);
            default:
                lexer_report_error(*lexer);
            }
        case 'L':
            switch (lexer_advance(lexer))
            {
            case 'S':
                return 0x00E0;
            default:
                lexer_report_error(*lexer);
            }
        default:
            lexer_report_error(*lexer);
        }
    case 'F':
        switch (lexer_advance(lexer))
            {
            case 'M':
                switch (lexer_advance(lexer))
                {
                case 'R':
                    return 0xF065 | lexer_read_register(lexer) << 8;
                default:
                    lexer_report_error(*lexer);
                }
            default:
                lexer_report_error(*lexer);
            }
        break;
    case 'J':
        switch (lexer_advance(lexer))
        {
        case 'M':
            switch (lexer_advance(lexer))
            {
            case 'P':
                return 0x1000 | lexer_read_12bit_number(lexer);
            default:
                lexer_report_error(*lexer);
            }
            break;
        case 'R':
            switch (lexer_advance(lexer))
            {
            case 'B':
                return 0xB000 | lexer_read_12bit_number(lexer);
            default:
                lexer_report_error(*lexer);
            }
        default:
            lexer_report_error(*lexer);
        }
        break;
    case 'M':
        switch (lexer_advance(lexer))
        {
        case 'O':
             switch (lexer_advance(lexer))
            {
            case 'V':
                 switch (lexer_peek(*lexer))
                {
                case 'A':
                    // MOVA
                    lexer_advance(lexer);
                    return 0x8002 | lexer_read_registers(lexer) << 4; 
                case 'O':
                    // MOVO
                    lexer_advance(lexer);
                    return 0x8001 | lexer_read_registers(lexer) << 4; 
                case 'S':
                    // MOVS
                    lexer_advance(lexer);
                    return 0x8007 | lexer_read_registers(lexer) << 4; 
                case 'X':
                    // MOVX
                    lexer_advance(lexer);
                    return 0x8003 | lexer_read_registers(lexer) << 4;        
                default:
                    // MOV
                    lexer_skip_whitespace(lexer);
                    switch (lexer_peek(*lexer))
                    {
                    case 'D':
                        switch (lexer_advance(lexer))
                        {
                        case 'T': // DT VX
                            return 0xF015 | lexer_read_register(lexer) << 8;
                        default:
                            lexer_report_error(*lexer);
                        }
                        break;
                    case 'I': // I 0xNNN
                        lexer_advance(lexer);
                        return 0xA000 | lexer_read_12bit_number(lexer);
                    case 'S':
                        switch (lexer_advance(lexer))
                        {
                        case 'T': // ST VX
                            return 0xF018 | lexer_read_register(lexer) << 8;
                        default:
                            lexer_report_error(*lexer);
                        }
                        break;
                    case 'V':
                    {
                        uint8_t registernumber = lexer_read_register(lexer);
                        lexer_skip_whitespace(lexer);
                        switch (lexer_advance(lexer))
                        {
                        case 'D':
                           switch (lexer_advance(lexer))
                            {
                                case 'T': // VX DT
                                    return 0xf007 | registernumber << 8;
                                default:
                                    lexer_report_error(*lexer);
                            }
                        case 'V': // VX VY
                            return 0x8000 | registernumber << 8 || lexer_read_register(lexer) << 4;
                        case '0':
                           switch (lexer_peek(*lexer))
                            {
                            case 'x':
                            case 'X': // VX 0xNN
                                return 0x6000 | registernumber << 8 | lexer_hexa(lexer, 2);
                            default:
                                lexer_report_error(*lexer);
                            }
                        default:
                            lexer_report_error(*lexer);
                        }
                    }
                    default:
                        lexer_report_error(*lexer);
                    }
                }        
            default:
                lexer_report_error(*lexer);
            }        
        default:
            lexer_report_error(*lexer);
        }
        break;
     case 'R':
        switch (lexer_advance(lexer))
            {
            case 'E':
                switch (lexer_advance(lexer))
                {
                case 'T': // RET
                    return 0x00EE;
                default:
                    lexer_report_error(*lexer);
                }
            default:
                lexer_report_error(*lexer);
            }
    case 'P':
        switch (lexer_advance(lexer))
            {
            case 'R':
                switch (lexer_advance(lexer))
                {
                case 'T': // PRT
                    return 0xF000 | lexer_read_register(lexer) << 8;
                default:
                    lexer_report_error(*lexer);
                }
                break;
            default:
                lexer_report_error(*lexer);
            }
    case 'S':
        switch (lexer_advance(lexer))
        {
        case 'K':
            switch (lexer_advance(lexer))
            {
            case 'E': //SKE
            {
                uint8_t registerNumber = lexer_read_register(lexer);
                lexer_skip_whitespace(lexer);
                if(lexer_peek(*lexer) == '0')
                return 0x3000 | registerNumber << 8 | lexer_read_8bit_number(lexer);
                if(lexer_peek(*lexer) == 'V')
                return 0x5000 | registerNumber << 8 | lexer_read_register(lexer) << 4;
            }
            case 'N':
                switch (lexer_advance(lexer))
                {
                case 'E': // SKNE - skip not equal
                    return 0x9000 | lexer_read_registers(lexer) << 4;
                case 'P': // SKNP - skip not pressed
                    return 0xE0A1 | lexer_read_register(lexer) << 8;
                default:
                    lexer_report_error(*lexer);
                }
            case 'P':
                return 0xE09E | lexer_read_register(lexer) << 8;
            default:
                lexer_report_error(*lexer);
            }
            break;
        case 'T':
            switch (lexer_advance(lexer))
            {            
            case 'B':
                switch (lexer_advance(lexer))
                {
                case 'C': // STBC
                    return 0xF033 | lexer_read_register(lexer) << 8;
                default:
                    lexer_report_error(*lexer);
                }
            case 'K': // STK
                return 0xF00A | lexer_read_register(lexer) << 8;
            case 'L':
                switch (lexer_advance(lexer))
                {
                case 'S': // STLS
                    return 0x8006 | lexer_read_registers(lexer) << 4;
                default:
                    lexer_report_error(*lexer);
                }
            case 'M':
                switch (lexer_advance(lexer))
                {
                case 'R': // STMR
                    return 0xF055 | lexer_read_registers(lexer) << 4;
                case 'S': // STMS
                    return 0x800E | lexer_read_registers(lexer) << 4;
                default:
                    lexer_report_error(*lexer);
                }
            default:
                lexer_report_error(*lexer);
            }
        case 'U':
            switch (lexer_advance(lexer))
            {
            case 'B': // SUB
                return 0x8005 | lexer_read_registers(lexer) << 4;
            default:
                lexer_report_error(*lexer);
            }        
        default:
            lexer_report_error(*lexer);
        }
    default:
        lexer_report_error(*lexer);
    }
}

static void lexer_skip_whitespace(lexer_t * lexer)
{
    for (;;)
    {
        char c = lexer_peek(*lexer);
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            // Whitespaces tabstops and carriage returns are ignored
            lexer_advance(lexer);
            break;
        case '\n':
            // Linecounter will increase on a linefeed
            lexer->line++;
            lexer_advance(lexer);
            break;
        case '#':
                // A comment goes until the end of the line.
            while (lexer_peek(*lexer) != '\n' && !lexer_is_at_end(*lexer))
                lexer_advance(lexer);
            break;
        default:
            return;
        }
    }
}