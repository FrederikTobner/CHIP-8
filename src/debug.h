#ifndef CHIP8_DEBUG_H
#define CHIP8_DEBUG_H

#include <stdint.h>

#include "chip8.h"

void debug_print_bytecode(uint16_t memoryLocation, uint16_t opcode);

void debug_trace_execution(chip8_t chip8);

#endif