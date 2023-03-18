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
 * @file debug.c
 * @brief Definitions regarding the debug functionality of the emulator
 */

#include "debug.h"

void debug_print_bytecode(uint16_t memoryLocation, uint16_t opcode) {
    printf("0x%04X: [0x%04X]\n", memoryLocation, opcode);
}

void debug_trace_execution(virtual_machine_t vm) {
    printf("Data registers: [");
    for (uint8_t i = 0; i < 16; i++) {
        printf("0x%02X, ", vm.V[i]);
    }
    printf("]\n");
    printf("Program counter: 0x%04X\n", vm.programCounter);
    printf("Current opcode: 0x%04X\n", vm.currentOpcode);
    printf("Address register: 0x%04X\n", vm.I);

    printf("Stack: [");
    uint16_t * upperBound = vm.stack + 16;
    for (uint16_t * stackPointer = vm.stack; stackPointer < upperBound; stackPointer++) {
        printf("0x%04X, ", *stackPointer);
    }
    printf("]\n");
    printf("Delay timer: 0x%02X\n", vm.delayTimer);
    printf("Sound timer: 0x%02X\n", vm.soundTimer);
}
