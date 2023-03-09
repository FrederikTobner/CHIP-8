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
 * @file assembler.c
 * @brief Definitions regarding the assembler of the emulator
 */

#include "assembler.h"
#include "../backend/chip8.h"

#define OPCODE_CONVERSION_ERROR_MESSAGE ("Unable to convert mnemonic at source into binary\n")

static inline char assembler_advance(assembler_t *);
static uint16_t assembler_convert_address_to_binary(assembler_t *);
static uint16_t assembler_convert_label_to_address(assembler_t *);
static uint16_t assembler_convert_mnemonic_to_binary(assembler_t *, char);
static uint8_t assembler_convert_register_to_binary(assembler_t *);
static uint8_t assembler_convert_registers_to_binary(assembler_t *);
static uint16_t assembler_hexa(assembler_t *, size_t);
static inline bool assembler_is_alpha(char);
static inline bool assembler_is_at_end(assembler_t);
static inline bool assembler_is_decimal(char);
static inline bool assembler_is_hexa(char);
static inline char assembler_peek(assembler_t);
static uint8_t assembler_read_8bit_number(assembler_t *);
static void assembler_report_error(assembler_t *, char const *, ...);
static void assembler_skip_whitespace(assembler_t *);
static int assembler_process_section(assembler_t *, chip8_t *, uint16_t *);
static void assembler_process_data_section(assembler_t *, chip8_t *, uint16_t *);
static void assembler_process_text_section(assembler_t *, chip8_t *, uint16_t *);
static void assembler_scan_label(assembler_t *, uint16_t);
static int32_t assembler_scan_opcode(assembler_t *);

void assembler_initialize(assembler_t * assembler, char const * source) {
    assembler->source = (char *)source;
    assembler->start = source;
    assembler->current = source;
    assembler->line = 1u;
    address_table_init_table(&assembler->table);
}

int assembler_process_file(assembler_t * assembler, chip8_t * chip8) {
    uint16_t memoryLocation = PROGRAM_START_LOCATION;
    assembler_skip_whitespace(assembler);
    while (!assembler_is_at_end(*assembler)) {
        if (assembler_process_section(assembler, chip8, &memoryLocation)) {
            return -1;
        }
    }
    free(assembler->source);
    // address_table_free_entries(&assembler->table);
    return 0;
}

/// @brief Processes a section in a chip8 assembly file
/// @param assembler  The assembler that processes the program
/// @param chip8 The emulator where the program will be executed
/// @param memoryLocation The memory-location where the section is stored
/// @return 0 if everything went well, -1 if an error occured
static int assembler_process_section(assembler_t * assembler, chip8_t * chip8, uint16_t * memoryLocation) {
    assembler_skip_whitespace(assembler);
    if (assembler_is_at_end(*assembler)) {
        return -1;
    }
    if (!strncmp(assembler->current, "section", 7)) {
        assembler->current += 7;
    } else {
        assembler_report_error(assembler, "No section in source file\n");
    }
    assembler_skip_whitespace(assembler);
    if (!strncmp(assembler->current, ".text:", 6)) {
        assembler_process_text_section(assembler, chip8, memoryLocation);
    } else if (!strncmp(assembler->current, ".data:", 6)) {
        assembler_process_data_section(assembler, chip8, memoryLocation);
    } else {
        return -1;
    }
    return 0;
}

/// @brief Processes the data section of a chip8 program
/// @param assembler The assembler that processes the section
/// @param chip8 The emulator where the program will be executed
/// @param memoryLocation The memory-location where the data is stored
static void assembler_process_data_section(assembler_t * assembler, chip8_t * chip8, uint16_t * memoryLocation) {
    assembler->current += 6;
    assembler_skip_whitespace(assembler);
    if (!strncmp(assembler->current, "org", 3)) {
        assembler->current += 3;
        assembler_skip_whitespace(assembler);
        uint16_t specifiedMemoryLocation = assembler_convert_address_to_binary(assembler);
        if (specifiedMemoryLocation < *memoryLocation) {
            fprintf(stderr, "Address specified in org collided with text segment\n");
            exit(EXIT_CODE_ASSEMBLER_ERROR);
        }
        *memoryLocation = specifiedMemoryLocation;
        assembler_skip_whitespace(assembler);
    } else {
        *memoryLocation += 2;
    }
    for (; !strncmp(assembler->current, "0x", 2) && *memoryLocation < 0xFFF; assembler_skip_whitespace(assembler)) {
        chip8_write_byte_to_memory(chip8, memoryLocation, assembler_read_8bit_number(assembler));
    }
    if (*memoryLocation > 0xFFF) {
        fprintf(stderr, "Data section is too big too be stored in memory\n");
        exit(EXIT_CODE_ASSEMBLER_ERROR);
    }
}

/// @brief Processes the text section of a chip8 program
/// @param assembler The assembler that processes the section
/// @param chip8 The emulator where the program will be executed
/// @param memoryLocation The memory-location where the code is stored
static void assembler_process_text_section(assembler_t * assembler, chip8_t * chip8, uint16_t * memoryLocation) {
    if (*memoryLocation != PROGRAM_START_LOCATION) {
        fprintf(stderr, "Text section must be declared before data section\n");
        exit(EXIT_CODE_ASSEMBLER_ERROR);
    }
    assembler->current += 6;
    int32_t opcode;
#ifdef PRINT_BYTE_CODE
    printf("=== Code ===\n");
#endif
    for (assembler_skip_whitespace(assembler); *memoryLocation <= 0xFFF && strncmp(assembler->current, "section", 7);
         assembler_skip_whitespace(assembler)) {
        if (assembler_peek(*assembler) == '_') {
            assembler_scan_label(assembler, *memoryLocation);
        } else {
            opcode = assembler_scan_opcode(assembler);
            if (opcode <= 0) {
                break;
            }
            chip8_write_opcode_to_memory(chip8, memoryLocation, opcode);
        }
    }
    if (*memoryLocation > 0xFFF) {
        fprintf(stderr, "Text section is too big too be stored in memory\n");
        exit(EXIT_CODE_ASSEMBLER_ERROR);
    }
}

/// @brief
/// @param assembler
/// @param memoryLocation
static void assembler_scan_label(assembler_t * assembler, uint16_t memoryLocation) {
    assembler_advance(assembler);
    char const * labelStart = assembler->current;
    char const * labelEnd = NULL;
    while (!assembler_is_at_end(*assembler)) {
        if (assembler_peek(*assembler) == ':') {
            break;
        }
        if (!assembler_is_alpha(assembler_peek(*assembler))) {
            assembler_report_error(assembler, "A label can only consist of alphanumeric characters");
        }
        labelEnd = assembler->current;
        assembler_advance(assembler);
    }
    if (!labelEnd) {
        return;
    }
    char * label = malloc(labelEnd - labelStart);
    if (!label) {
        return;
    }
    memcpy(label, labelStart, labelEnd - labelStart + 1);
    label[labelEnd - labelStart + 1] = '\0';
    address_hash_table_entry_t * entry = address_table_entry_new(memoryLocation, label);
    if (!entry) {
        return;
    }
    address_table_insert_entry(entry, &assembler->table);
    assembler_advance(assembler);
}

/// @brief Scans the next opcode in the sourcefile
/// @param assembler The assembler where the next opcode is scanned
/// @return The opcode or -1 if we have reached the end of the file or the opcode wasn't processed properly
static int32_t assembler_scan_opcode(assembler_t * assembler) {
    if (assembler_is_at_end(*assembler)) {
        return -1;
    }
    assembler->start = assembler->current;
    char c = assembler_advance(assembler);
    if (assembler_is_alpha(c)) {
        return assembler_convert_mnemonic_to_binary(assembler, c);
    }
    if (c == '0' && (assembler_peek(*assembler) == 'x' || assembler_peek(*assembler) == 'X')) {
        return assembler_hexa(assembler, 4);
    }
    return -1;
}

/// @brief Advances a position further in the sourceCode and returns the prevoius Token
/// @param assembler Advances a position further in the sourcecode
/// @return The current character
static inline char assembler_advance(assembler_t * assembler) {
    return *assembler->current++;
}

/// @brief Converts the mnemonic representation of a number in the hexadecimal formtat from the the sourcefile to it's
/// representation in binary
/// @param assembler The assembler struct that is used to store the state of the process
/// @param digitCount The amount of digits the number has
/// @return The binary representation of the number
static uint16_t assembler_hexa(assembler_t * assembler, size_t digitCount) {
    assembler->current++;
    uint16_t number = 0;
    for (size_t i = 0; i < digitCount; i++) {
        number *= 16;
        if (!assembler_is_hexa(assembler_peek(*assembler))) {
            assembler_report_error(assembler, "Invalid character in hexadecimal sequence\n");
        }
        if (assembler_is_decimal(assembler_peek(*assembler))) {
            number += *assembler->current++ - '0';
        } else if (assembler_peek(*assembler) <= 'F') {
            number += *assembler->current++ - 'A' + 10;
        } else {
            number += *assembler->current++ - 'a' + 10;
        }
    }
    return number;
}

/// @brief Checks if the char c is a from the alphabet
/// @param c The character that is checked
/// @return true if the char is from the aphabet, false if not
static inline bool assembler_is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

/// @brief Determines wheather we reached the end in the sourceCode
/// @param assembler The assembler where the current position is checked
/// @return true if the end of the sourcefile has been reached, false if not
static inline bool assembler_is_at_end(assembler_t assembler) {
    return *assembler.current == '\0';
}

/// @brief Checks if the char c is a decimal digit
/// @param c The character that is checked
/// @return true if the char is a decimal digit, false if not
static inline bool assembler_is_decimal(char c) {
    return c >= '0' && c <= '9';
}

/// @brief Checks if the char c is a hexadecimal digit
/// @param c The character that is checked
/// @return true if the char is a hexadecimal digit, false if not
static inline bool assembler_is_hexa(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/// @brief Used to get the next character in the sourcefile without advancing
/// @param assembler The assembler struct that is used to track the process
/// @return The next character in the sourcefile
static inline char assembler_peek(assembler_t assembler) {
    return *assembler.current;
}

/// @brief Converts the mnemnic representation of a byte to binary
/// @param assembler The assembler that proceeses the assembly file
/// @return The binary representation of the byte
static uint8_t assembler_read_8bit_number(assembler_t * assembler) {
    assembler_skip_whitespace(assembler);
    char c = assembler_advance(assembler);
    if (c == '0' && (assembler_peek(*assembler) == 'x' || assembler_peek(*assembler) == 'X')) {
        return assembler_hexa(assembler, 2);
    }
    assembler_report_error(assembler, "Could not parse 8 bit number\n");
// Unreachable
#if defined(COMPILER_MSVC)
    __assume(0);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    __builtin_unreachable();
#else // Compiler is Intel or unknown
    return 0;
#endif
}

/// @brief Converts the mnemnic representation of a address to binary
/// @param assembler The assembler that proceeses the assembly file
/// @return The binary representation of the address
static uint16_t assembler_convert_address_to_binary(assembler_t * assembler) {
    assembler_skip_whitespace(assembler);
    char c = assembler_advance(assembler);
    if (c == '0' && (assembler_peek(*assembler) == 'x' || assembler_peek(*assembler) == 'X')) {
        return assembler_hexa(assembler, 3);
    }
    assembler_report_error(assembler, "Unable to convert address\n");
// Unreachable
#if defined(COMPILER_MSVC)
    __assume(0);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    __builtin_unreachable();
#else // Compiler is Intel or unknown
    return 0;
#endif
}

/// @brief
/// @param assembler
/// @return
static uint16_t assembler_convert_label_to_address(assembler_t * assembler) {
    char const * labelStart = assembler->current;
    char const * labelEnd = NULL;
    for (; !assembler_is_at_end(*assembler) && assembler_is_alpha(assembler_peek(*assembler));
         labelEnd = assembler->current, assembler_advance(assembler)) {
    }
    char * label = malloc(labelEnd - labelStart - 2);
    if (!label) {
        assembler_report_error(assembler, "Unable to allocate memory for label");
    }
    memcpy(label, labelStart, labelEnd - labelStart + 1);
    label[labelEnd - labelStart + 1] = '\0';
    uint16_t opCode = address_table_look_up_entry(label, &assembler->table)->opcodeAddress;
    return opCode;
}

/// @brief Converts the mnemnic representation of a register to binary
/// @param assembler The assembler that proceeses the assembly file
/// @return The binary representation of the register
static uint8_t assembler_convert_register_to_binary(assembler_t * assembler) {
    assembler_skip_whitespace(assembler);
    switch (assembler_advance(assembler)) {
    case 'V':
        if (!assembler_is_hexa(assembler_peek(*assembler))) {
            assembler_report_error(assembler, "Unable to read register\n");
        }
        assembler->current--;
        return assembler_hexa(assembler, 1);
    default:
        assembler_report_error(assembler, "Unable to read register\n");
        break;
    }
// Unreachable
#if defined(COMPILER_MSVC)
    __assume(0);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    __builtin_unreachable();
#else // Compiler is Intel or unknown
    return 0;
#endif
}

/// @brief Converts the mnemnic representation of a registers to binary
/// @param assembler The assembler that proceeses the assembly file
/// @return The binary representation of the registers
static uint8_t assembler_convert_registers_to_binary(assembler_t * assembler) {
    uint8_t value = assembler_convert_register_to_binary(assembler) << 4;
    return value += assembler_convert_register_to_binary(assembler);
}

/// @brief Reports an error when the assembly waas processed
/// @param assembler The assembler where the error occured
static inline void assembler_report_error(assembler_t * assembler, char const * format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    free(assembler->source);
    exit(EXIT_CODE_ASSEMBLER_ERROR);
}

/// @brief Converts the next mnemonic representation of an opcode in the source file to it's binary representation
/// @param assembler The assembler that proceeses the assembly file
/// @param c The next character in the source code
/// @return The mnemonic representation converted to the representation in binary
static uint16_t assembler_convert_mnemonic_to_binary(assembler_t * assembler, char c) {
    switch (c) {
    case 'A':
        switch (assembler_advance(assembler)) {
        case 'D':
            switch (assembler_advance(assembler)) {
            case 'D':
                assembler_skip_whitespace(assembler);
                switch (assembler_peek(*assembler)) {
                case 'I': // ADD I
                    assembler_advance(assembler);
                    return 0xF01E | assembler_convert_register_to_binary(assembler) << 8;
                case 'V': // ADD V
                    return 0x8004 | assembler_convert_registers_to_binary(assembler) << 4;
                default:
                    assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                }
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
    case 'C':
        switch (assembler_advance(assembler)) {
        case 'A':
            switch (assembler_advance(assembler)) {
            case 'L': // CAL
                return 0x2000 | assembler_convert_address_to_binary(assembler);
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        case 'L':
            switch (assembler_advance(assembler)) {
            case 'S': // CLS
                return 0x00E0;
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
    case 'D':
        switch (assembler_advance(assembler)) {
        case 'S':
            switch (assembler_advance(assembler)) {
            case 'P': // DSP
                return (0xD000 | assembler_convert_registers_to_binary(assembler) << 4) |
                       (assembler_read_8bit_number(assembler) & 0xf);
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
    case 'F':
        switch (assembler_advance(assembler)) {
        case 'M':
            switch (assembler_advance(assembler)) {
            case 'R': // FMR
                return 0xF065 | assembler_convert_register_to_binary(assembler) << 8;
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
        break;
    case 'J':
        switch (assembler_advance(assembler)) {
        case 'M':
            switch (assembler_advance(assembler)) {
            case 'P':
                {
                    assembler_skip_whitespace(assembler);
                    if (assembler_is_alpha(assembler_peek(*assembler))) {
                        return 0x1000 | assembler_convert_label_to_address(assembler);
                    } else {
                        return 0x1000 | assembler_convert_address_to_binary(assembler);
                    }
                }
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
            break;
        case 'R':
            switch (assembler_advance(assembler)) {
            case 'B':
                return 0xB000 | assembler_convert_address_to_binary(assembler);
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
        break;
    case 'M':
        switch (assembler_advance(assembler)) {
        case 'O':
            switch (assembler_advance(assembler)) {
            case 'V':
                switch (assembler_peek(*assembler)) {
                case 'A':
                    // MOVA
                    assembler_advance(assembler);
                    return 0x8002 | assembler_convert_registers_to_binary(assembler) << 4;
                case 'O':
                    // MOVO
                    assembler_advance(assembler);
                    return 0x8001 | assembler_convert_registers_to_binary(assembler) << 4;
                case 'S':
                    // MOVS
                    assembler_advance(assembler);
                    return 0x8007 | assembler_convert_registers_to_binary(assembler) << 4;
                case 'X':
                    // MOVX
                    assembler_advance(assembler);
                    return 0x8003 | assembler_convert_registers_to_binary(assembler) << 4;
                default:
                    // MOV
                    assembler_skip_whitespace(assembler);
                    switch (assembler_peek(*assembler)) {
                    case 'D':
                        switch (assembler_advance(assembler)) {
                        case 'T': // DT VX
                            return 0xF015 | assembler_convert_register_to_binary(assembler) << 8;
                        default:
                            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                        }
                        break;
                    case 'I': // I 0xNNNN
                        assembler_advance(assembler);
                        return 0xA000 | assembler_convert_address_to_binary(assembler);
                    case 'S':
                        switch (assembler_advance(assembler)) {
                        case 'T': // ST VX
                            return 0xF018 | assembler_convert_register_to_binary(assembler) << 8;
                        default:
                            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                        }
                        break;
                    case 'V':
                        {
                            uint8_t registernumber = assembler_convert_register_to_binary(assembler);
                            assembler_skip_whitespace(assembler);
                            switch (assembler_advance(assembler)) {
                            case 'D':
                                switch (assembler_advance(assembler)) {
                                case 'T': // VX DT
                                    return 0xf007 | registernumber << 8;
                                default:
                                    assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                                }
                            case 'V': // VX VY
                                return 0x8000 | registernumber << 8 || assembler_convert_register_to_binary(assembler)
                                                                           << 4;
                            case '0':
                                switch (assembler_peek(*assembler)) {
                                case 'x':
                                case 'X': // VX 0xNN
                                    return 0x6000 | registernumber << 8 | assembler_hexa(assembler, 2);
                                default:
                                    assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                                }
                            default:
                                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                            }
                        }
                    default:
                        assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                    }
                }
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
        break;
    case 'N':
        switch (assembler_advance(assembler)) {
        case 'O':
            switch (assembler_advance(assembler)) {
            case 'P': // NOP
                return 0x0001;
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
            break;
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
    case 'R':
        switch (assembler_advance(assembler)) {
        case 'E':
            switch (assembler_advance(assembler)) {
            case 'T': // RET
                return 0x00EE;
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        case 'N':
            switch (assembler_advance(assembler)) {
            case 'D': // RND
                return 0xC000 | assembler_convert_register_to_binary(assembler) << 8 |
                       assembler_read_8bit_number(assembler);
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
    case 'P':
        switch (assembler_advance(assembler)) {
        case 'R':
            switch (assembler_advance(assembler)) {
            case 'T': // PRT
                return 0xF000 | assembler_convert_register_to_binary(assembler) << 8;
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
            break;
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
    case 'S':
        switch (assembler_advance(assembler)) {
        case 'K':
            switch (assembler_advance(assembler)) {
            case 'E': // SKE
                {
                    uint8_t registerNumber = assembler_convert_register_to_binary(assembler);
                    assembler_skip_whitespace(assembler);
                    if (assembler_peek(*assembler) == '0') {
                        return 0x3000 | registerNumber << 8 | assembler_read_8bit_number(assembler);
                    }
                    if (assembler_peek(*assembler) == 'V') {
                        return 0x5000 | registerNumber << 8 | assembler_convert_register_to_binary(assembler) << 4;
                    }
                }
            case 'N':
                switch (assembler_advance(assembler)) {
                case 'E': // SKNE - skip not equal
                    return 0x9000 | assembler_convert_registers_to_binary(assembler) << 4;
                case 'P': // SKNP - skip not pressed
                    return 0xE0A1 | assembler_convert_register_to_binary(assembler) << 8;
                default:
                    assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                }
            case 'P':
                return 0xE09E | assembler_convert_register_to_binary(assembler) << 8;
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
            break;
        case 'T':
            switch (assembler_advance(assembler)) {
            case 'B':
                switch (assembler_advance(assembler)) {
                case 'C': // STBC
                    return 0xF033 | assembler_convert_register_to_binary(assembler) << 8;
                default:
                    assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                }
            case 'K': // STK
                return 0xF00A | assembler_convert_register_to_binary(assembler) << 8;
            case 'L':
                switch (assembler_advance(assembler)) {
                case 'S': // STLS
                    return 0x8006 | assembler_convert_registers_to_binary(assembler) << 4;
                default:
                    assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                }
            case 'M':
                switch (assembler_advance(assembler)) {
                case 'R': // STMR
                    return 0xF055 | assembler_convert_registers_to_binary(assembler) << 4;
                case 'S': // STMS
                    return 0x800E | assembler_convert_registers_to_binary(assembler) << 4;
                default:
                    assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
                }
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        case 'U':
            switch (assembler_advance(assembler)) {
            case 'B': // SUB
                return 0x8005 | assembler_convert_registers_to_binary(assembler) << 4;
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
    case 'T':
        switch (assembler_advance(assembler)) {
        case 'G':
            switch (assembler_advance(assembler)) {
            case 'S': // TGS
                return 0x00E1;
            default:
                assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
            }
        default:
            assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
        }
    default:
        assembler_report_error(assembler, OPCODE_CONVERSION_ERROR_MESSAGE);
    }
// Unreachable
#if defined(COMPILER_MSVC)
    __assume(0);
#elif defined(COMPILER_GCC) || defined(COMPILER_CLANG)
    __builtin_unreachable();
#else
    return 0;
#endif
}

/// Skips all the whitespace characters in the source file
/// @param assembler The assembler where the whitespace's are skipped at the position in the source file that is
/// currently processed
static void assembler_skip_whitespace(assembler_t * assembler) {
    for (;;) {
        char c = assembler_peek(*assembler);
        switch (c) {
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
            while (assembler_peek(*assembler) != '\n' && !assembler_is_at_end(*assembler)) {
                assembler_advance(assembler);
            }
            break;
        default:
            return;
        }
    }
}
