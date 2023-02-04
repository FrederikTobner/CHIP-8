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

#include "assembler.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"

static inline char assembler_advance(assembler_t * assembler);
static uint16_t assembler_hexa(assembler_t * assembler, size_t digitCount);
static inline bool assembler_is_alpha(char c);
static inline bool assembler_is_at_end(assembler_t assembler);
static inline bool assembler_is_decimal(char c);
static inline bool assembler_is_hexa(char c);
static inline char assembler_peek(assembler_t assembler);
static uint8_t assembler_read_8bit_number(assembler_t * assembler);
static uint16_t assembler_read_12bit_number(assembler_t * assembler);
static uint8_t assembler_read_register(assembler_t * assembler);
static uint8_t assembler_read_registers(assembler_t * assembler);
static inline void assembler_report_error(assembler_t assembler);
static uint16_t assembler_scan_mnemonic(assembler_t * assembler, char c);
static void assembler_skip_whitespace(assembler_t * assembler);

/// @brief Initialzes the assembler
/// @param assembler The assembler that is inialized
/// @param source The content of the source file that is processed by the assembler
void assembler_initialize(assembler_t * assembler, char const * source)
{
    assembler->start = source;
    assembler->current = source;
    assembler->line = 1u;
}

/// @brief Scans the next opcode in the sourcefile
/// @param assembler The assembler where the next opcode is scanned
/// @return The opcode or -1 if we have reached the end of the file or the opcode wasn't processed properly
int32_t assembler_scan_opcode(assembler_t * assembler)
{
    assembler_skip_whitespace(assembler);
    if (assembler_is_at_end(*assembler))
        return -1;
    assembler->start = assembler->current;
    char c = assembler_advance(assembler);
    if(assembler_is_alpha(c))
        return assembler_scan_mnemonic(assembler, c);
    if (c == '0' && (assembler_peek(*assembler) == 'x' || assembler_peek(*assembler) == 'X'))
        return assembler_hexa(assembler, 4);
    return -1;
}

/// @brief Advances a position further in the sourceCode and returns the prevoius Token
/// @param assembler Advances a position further in the sourcecode
/// @return The current character
static inline char assembler_advance(assembler_t * assembler)
{
    return *assembler->current++;
}
    
static uint16_t assembler_hexa(assembler_t * assembler, size_t digitCount)
{
    assembler->current++;
    uint16_t number = 0;
    for (size_t i = 0; i < digitCount; i++)
    {
        number *= 16;
        if(!assembler_is_hexa(assembler_peek(*assembler)))
            assembler_report_error(*assembler);
        if(assembler_is_decimal(assembler_peek(*assembler)))
            number += *assembler->current++ - '0';
        else if(assembler_peek(*assembler) <= 'F')
            number += *assembler->current++ - 'A' + 10;
        else
            number += *assembler->current++ - 'a' + 10;
    }
    return number;    
}

// checks if the char c is a from the alphabet
static inline bool assembler_is_alpha(char c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

// Determines wheather we reached the end in the sourceCode
static inline bool assembler_is_at_end(assembler_t assembler)
{
    return *assembler.current == '\0';
}

// checks if the char c is a digit
static inline bool assembler_is_decimal(char c)
{
    return c >= '0' && c <= '9';
}

// checks if the char c is a hexadecimal digit
static inline bool assembler_is_hexa(char c)
{
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

static inline char assembler_peek(assembler_t assembler)
{
    return *assembler.current;
}

static uint8_t assembler_read_8bit_number(assembler_t * assembler)
{
    assembler_skip_whitespace(assembler);
    char c = assembler_advance(assembler);
    if (c == '0' && (assembler_peek(*assembler) == 'x' || assembler_peek(*assembler) == 'X'))
        return assembler_hexa(assembler, 2);
    assembler_report_error(*assembler);
}

static uint16_t assembler_read_12bit_number(assembler_t * assembler)
{
    assembler_skip_whitespace(assembler);
    char c = assembler_advance(assembler);
    if (c == '0' && (assembler_peek(*assembler) == 'x' || assembler_peek(*assembler) == 'X'))
        return assembler_hexa(assembler, 3);
    assembler_report_error(*assembler);
}

static uint8_t assembler_read_register(assembler_t * assembler)
{
    assembler_skip_whitespace(assembler);
    switch (assembler_advance(assembler))
    {
        case 'V':
            if(!assembler_is_hexa(assembler_peek(*assembler)))
                assembler_report_error(*assembler);
            assembler->current--;
            return assembler_hexa(assembler, 1);
        default:
            assembler_report_error(*assembler);
            break;
    }
}

static uint8_t assembler_read_registers(assembler_t * assembler)
{
    uint8_t value = assembler_read_register(assembler);
    value <<= 4;
    
    return value += assembler_read_register(assembler);
}

static inline void assembler_report_error(assembler_t assembler)
{
    printf("Unexpected character %c in opcode sequence in line %i", *assembler.current, assembler.line);
    exit(EXIT_CODE_COMPILATION_ERROR);
}

static uint16_t assembler_scan_mnemonic(assembler_t * assembler, char c)
{
    switch (c)
    {        
    case 'A':
        switch (assembler_advance(assembler))
            {
            case 'D':
                switch (assembler_advance(assembler))
                {
                case 'D':
                    assembler_skip_whitespace(assembler);
                    switch (assembler_peek(*assembler))
                    {
                    case 'I':
                        assembler_advance(assembler);
                        return 0xF01E | assembler_read_register(assembler) << 8;
                    case 'V':
                        return 0x8004 |assembler_read_registers(assembler) << 4;                    
                    default:
                        assembler_report_error(*assembler);
                    }
                default:
                    assembler_report_error(*assembler);
                }
            default:
                assembler_report_error(*assembler);
            }
    case 'C':
        switch (assembler_advance(assembler))
        {
        case 'A':
            switch (assembler_advance(assembler))
            {
            case 'L':
                return 0x2000 | assembler_read_12bit_number(assembler);
            default:
                assembler_report_error(*assembler);
            }
        case 'L':
            switch (assembler_advance(assembler))
            {
            case 'S':
                return 0x00E0;
            default:
                assembler_report_error(*assembler);
            }
        default:
            assembler_report_error(*assembler);
        }
    case 'F':
        switch (assembler_advance(assembler))
            {
            case 'M':
                switch (assembler_advance(assembler))
                {
                case 'R':
                    return 0xF065 | assembler_read_register(assembler) << 8;
                default:
                    assembler_report_error(*assembler);
                }
            default:
                assembler_report_error(*assembler);
            }
        break;
    case 'J':
        switch (assembler_advance(assembler))
        {
        case 'M':
            switch (assembler_advance(assembler))
            {
            case 'P':
                return 0x1000 | assembler_read_12bit_number(assembler);
            default:
                assembler_report_error(*assembler);
            }
            break;
        case 'R':
            switch (assembler_advance(assembler))
            {
            case 'B':
                return 0xB000 | assembler_read_12bit_number(assembler);
            default:
                assembler_report_error(*assembler);
            }
        default:
            assembler_report_error(*assembler);
        }
        break;
    case 'M':
        switch (assembler_advance(assembler))
        {
        case 'O':
             switch (assembler_advance(assembler))
            {
            case 'V':
                 switch (assembler_peek(*assembler))
                {
                case 'A':
                    // MOVA
                    assembler_advance(assembler);
                    return 0x8002 | assembler_read_registers(assembler) << 4; 
                case 'O':
                    // MOVO
                    assembler_advance(assembler);
                    return 0x8001 | assembler_read_registers(assembler) << 4; 
                case 'S':
                    // MOVS
                    assembler_advance(assembler);
                    return 0x8007 | assembler_read_registers(assembler) << 4; 
                case 'X':
                    // MOVX
                    assembler_advance(assembler);
                    return 0x8003 | assembler_read_registers(assembler) << 4;        
                default:
                    // MOV
                    assembler_skip_whitespace(assembler);
                    switch (assembler_peek(*assembler))
                    {
                    case 'D':
                        switch (assembler_advance(assembler))
                        {
                        case 'T': // DT VX
                            return 0xF015 | assembler_read_register(assembler) << 8;
                        default:
                            assembler_report_error(*assembler);
                        }
                        break;
                    case 'I': // I 0xNNN
                        assembler_advance(assembler);
                        return 0xA000 | assembler_read_12bit_number(assembler);
                    case 'S':
                        switch (assembler_advance(assembler))
                        {
                        case 'T': // ST VX
                            return 0xF018 | assembler_read_register(assembler) << 8;
                        default:
                            assembler_report_error(*assembler);
                        }
                        break;
                    case 'V':
                    {
                        uint8_t registernumber = assembler_read_register(assembler);
                        assembler_skip_whitespace(assembler);
                        switch (assembler_advance(assembler))
                        {
                        case 'D':
                           switch (assembler_advance(assembler))
                            {
                                case 'T': // VX DT
                                    return 0xf007 | registernumber << 8;
                                default:
                                    assembler_report_error(*assembler);
                            }
                        case 'V': // VX VY
                            return 0x8000 | registernumber << 8 || assembler_read_register(assembler) << 4;
                        case '0':
                           switch (assembler_peek(*assembler))
                            {
                            case 'x':
                            case 'X': // VX 0xNN
                                return 0x6000 | registernumber << 8 | assembler_hexa(assembler, 2);
                            default:
                                assembler_report_error(*assembler);
                            }
                        default:
                            assembler_report_error(*assembler);
                        }
                    }
                    default:
                        assembler_report_error(*assembler);
                    }
                }        
            default:
                assembler_report_error(*assembler);
            }        
        default:
            assembler_report_error(*assembler);
        }
        break;
     case 'R':
        switch (assembler_advance(assembler))
            {
            case 'E':
                switch (assembler_advance(assembler))
                {
                case 'T': // RET
                    return 0x00EE;
                default:
                    assembler_report_error(*assembler);
                }
            default:
                assembler_report_error(*assembler);
            }
    case 'P':
        switch (assembler_advance(assembler))
            {
            case 'R':
                switch (assembler_advance(assembler))
                {
                case 'T': // PRT
                    return 0xF000 | assembler_read_register(assembler) << 8;
                default:
                    assembler_report_error(*assembler);
                }
                break;
            default:
                assembler_report_error(*assembler);
            }
    case 'S':
        switch (assembler_advance(assembler))
        {
        case 'K':
            switch (assembler_advance(assembler))
            {
            case 'E': //SKE
            {
                uint8_t registerNumber = assembler_read_register(assembler);
                assembler_skip_whitespace(assembler);
                if(assembler_peek(*assembler) == '0')
                return 0x3000 | registerNumber << 8 | assembler_read_8bit_number(assembler);
                if(assembler_peek(*assembler) == 'V')
                return 0x5000 | registerNumber << 8 | assembler_read_register(assembler) << 4;
            }
            case 'N':
                switch (assembler_advance(assembler))
                {
                case 'E': // SKNE - skip not equal
                    return 0x9000 | assembler_read_registers(assembler) << 4;
                case 'P': // SKNP - skip not pressed
                    return 0xE0A1 | assembler_read_register(assembler) << 8;
                default:
                    assembler_report_error(*assembler);
                }
            case 'P':
                return 0xE09E | assembler_read_register(assembler) << 8;
            default:
                assembler_report_error(*assembler);
            }
            break;
        case 'T':
            switch (assembler_advance(assembler))
            {            
            case 'B':
                switch (assembler_advance(assembler))
                {
                case 'C': // STBC
                    return 0xF033 | assembler_read_register(assembler) << 8;
                default:
                    assembler_report_error(*assembler);
                }
            case 'K': // STK
                return 0xF00A | assembler_read_register(assembler) << 8;
            case 'L':
                switch (assembler_advance(assembler))
                {
                case 'S': // STLS
                    return 0x8006 | assembler_read_registers(assembler) << 4;
                default:
                    assembler_report_error(*assembler);
                }
            case 'M':
                switch (assembler_advance(assembler))
                {
                case 'R': // STMR
                    return 0xF055 | assembler_read_registers(assembler) << 4;
                case 'S': // STMS
                    return 0x800E | assembler_read_registers(assembler) << 4;
                default:
                    assembler_report_error(*assembler);
                }
            default:
                assembler_report_error(*assembler);
            }
        case 'U':
            switch (assembler_advance(assembler))
            {
            case 'B': // SUB
                return 0x8005 | assembler_read_registers(assembler) << 4;
            default:
                assembler_report_error(*assembler);
            }        
        default:
            assembler_report_error(*assembler);
        }
    default:
        assembler_report_error(*assembler);
    }
}

static void assembler_skip_whitespace(assembler_t * assembler)
{
    for (;;)
    {
        char c = assembler_peek(*assembler);
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            // Whitespaces tabstops and carriage returns are ignored
            assembler_advance(assembler);
            break;
        case '\n':
            // Linecounter will increase on a linefeed
            assembler->line++;
            assembler_advance(assembler);
            break;
        case '#':
                // A comment goes until the end of the line.
            while (assembler_peek(*assembler) != '\n' && !assembler_is_at_end(*assembler))
                assembler_advance(assembler);
            break;
        default:
            return;
        }
    }
}
