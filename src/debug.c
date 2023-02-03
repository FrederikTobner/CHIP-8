#include "debug.h"

#include <stdio.h>

void debug_print_bytecode(uint16_t memoryLocation, uint16_t opcode)
{
    printf("0x%04X: [0x%04X]\n", memoryLocation, opcode);
}

void debug_trace_execution(chip8_t chip8)
{
    printf("Data registers: [");
    for (uint_fast8_t i = 0; i < 16; i++)
    {
        printf("0x%02X, ", chip8.V[i]);
    }
    printf("]\n");

    printf("Program counter: 0x%04X\n", chip8.programCounter);
    printf("Current opcode: 0x%04X\n", chip8.currentOpcode);
    printf("Address register: 0x%04X\n", chip8.I);
    
    printf("Stack: [");
    uint16_t * upperBound = chip8.stack + 16;
    for (uint16_t * stackPointer = chip8.stack; stackPointer < upperBound; stackPointer++)
        printf("0x%04X, ", *stackPointer);
    printf("]\n");
    printf("Delay timer: 0x%02X\n", chip8.delay_timer);
    printf("Sound timer: 0x%02X\n", chip8.sound_timer);
}