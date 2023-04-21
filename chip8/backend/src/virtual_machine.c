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
 * @file virtual_machine.c
 * @brief Definitions regarding the virtual machine of the emulator
 */

#include "virtual_machine.h"

#if defined(PRINT_BYTE_CODE) || defined(TRACE_EXECUTION)
#include "debug.h"
#endif
#include "../../base/src/chip8.h"
#include "../../base/src/logger.h"
#include "display.h"
#include "keyboard_state.h"

/// Defines a new 8-bit value based on the opcode that is currently executed
#define DEFINE_8_BIT_VALUE  uint8_t value = vm->currentOpcode & 0x00ff;

/// Defines a new 12-bit value based on the opcode that is currently executed
#define DEFINE_12_BIT_VALUE uint16_t value = vm->currentOpcode & 0x0fff;

/// Defines a new 4-bit value based on the opcode that is currently executed
#define DEFINE_X            uint8_t x = (vm->currentOpcode & 0x0f00u) >> 8;

/// Defines a new 4-bit value based on the opcode that is currently executed
#define DEFINE_Y            uint8_t y = (vm->currentOpcode & 0x00f0u) >> 4;

/// The clock speed of the CHIP-8 CPU (600 Hz)
#define CHIP8_CLOCK_SPEED   (600.0)

/// The character sprites that are stored in memory (from 0x)
#define CHARACTER_SPRITES                                                                                           \
    ("\xF0\x90\x90\x90\xF0\x20\x60\x20\x20\x70\xF0\x10\xF0\x80\xF0\xF0\x10\xF0\x10\xF0\x90\x90\xF0\x10\x10\xF0\x80" \
     "\xF0\x10\xF0\xF0\x80\xF0\x90\xF0\xF0\x10\x20\x40\x40\xF0\x90\xF0\x90\xF0\xF0\x90\xF0\x10\xF0\xF0\x90\xF0\x90" \
     "\x90\xE0\x90\xE0\x90\xE0\xF0\x80\x80\x80\xF0\xE0\x90\x90\x90\xE0\xF0\x80\xF0\x80\xF0\xF0\x80\xF0\x80\x80")

static int8_t virtual_machine_execute_next_opcode(virtual_machine_t *, keyBoardState_t);
static inline void virtual_machine_place_character_sprites_in_memory(virtual_machine_t *);

/// @brief Executes the program that is stored in memory
/// @param vm The chip8 vm where the program that is currently held in memory is executed
void virtual_machine_execute(virtual_machine_t * vm) {
    clock_t last_t, current_t = clock();
    uint64_t numberofChip8Clocks = 0;
    SDL_Event event;
    keyBoardState_t keyBoardState = 0;
    double secondsElapsed;
    for (; vm->programCounter < ((0x1000 - PROGRAM_START_LOCATION) / 2); vm->programCounter++, numberofChip8Clocks++) {
        last_t = current_t;
        // Polling SDL events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                return;
            case SDL_KEYDOWN:
                keyboard_handle_key_down_event(event, &keyBoardState);
                break;
            case SDL_KEYUP:
                keyboard_handle_key_up_event(event, &keyBoardState);
                break;
            default:
                break;
            }
        }
#ifdef TRACE_EXECUTION
        debug_trace_execution(*vm);
#endif
        vm->currentOpcode = (uint16_t)vm->memory[vm->programCounter * 2 + 1 + PROGRAM_START_LOCATION];
        vm->currentOpcode += vm->memory[vm->programCounter * 2 + PROGRAM_START_LOCATION] << 8;
        // Reached end of the program
        if (!vm->currentOpcode) {
            return;
        }
        // Executes next opcode
        if (virtual_machine_execute_next_opcode(vm, keyBoardState)) {
            return;
        }
        // 60 hz
        if (!(numberofChip8Clocks % 10)) {
            if (vm->delayTimer) {
                vm->delayTimer--;
            }
            if (vm->soundTimer) {
                putc('\a', stdout);
                vm->soundTimer--;
            }
            display_render(vm->display);
        }
        // Wait for a 1/600 second minus the time elapsed
        current_t = clock();
        secondsElapsed = (double)(current_t - last_t);
        if (1.0 / CHIP8_CLOCK_SPEED > secondsElapsed) {
            // Lets pretend SDL_Delay does not exist
#if defined(OS_WINDOWS)
            // Milliseconds -> multiply with 1000
            Sleep((1.0 / CHIP8_CLOCK_SPEED - secondsElapsed) * 1000.0);
#elif defined(OS_UNIX_LIKE)
            // Mircoseconds -> multiply with 1000000
            usleep((1.0 / CHIP8_CLOCK_SPEED - secondsElapsed) * 10000000.0);
#endif
        }
    }
}

/// @brief Initializes the chip8 vm
/// @param vm The chip8 virtual machine that is initialzed
void virtual_machine_init(virtual_machine_t * vm) {
    uint8_t * upperBound = (vm->V + 16u);
    for (uint8_t * memoryPointer = vm->V; memoryPointer < upperBound; memoryPointer++) {
        *memoryPointer = 0u;
    }
    // Initialize stackpointer
    vm->stackPointer = vm->stack;
    // Initialize program counter
    vm->programCounter = 0u;
    // Compute upper bound for memory loop
    upperBound = (vm->memory + 4096u);
    // Initialize memory
    for (uint8_t * memoryPointer = vm->memory; memoryPointer < upperBound; memoryPointer++) {
        *memoryPointer = 0u;
    }
    // Setting up Hex character sprites in memory
    virtual_machine_place_character_sprites_in_memory(vm);
}

/// @brief Writtes the specified opcode at the specified location into memory
/// @param vm The chip8 where the opcode is written to memory
/// @param memoryLocation The location where the opcode is written to (0-4096)
/// @param opcode The opcode that is written into memory
void virtual_machine_write_opcode_to_memory(virtual_machine_t * vm, uint16_t * memoryLocation, uint16_t opcode) {
    if (*memoryLocation > 0x1000u || *memoryLocation < PROGRAM_START_LOCATION) {
        return;
    }
#ifdef PRINT_BYTE_CODE
    debug_print_bytecode(*memoryLocation, opcode);
#endif
    vm->memory[(*memoryLocation)++] = (opcode & 0xff00) >> 8;
    vm->memory[(*memoryLocation)++] = opcode & 0x00ff;
}

/// @brief Writtes the specified opcode at the specified location into memory
/// @param vm The chip8 where the opcode is written to memory
/// @param memoryLocation The location where the opcode is written to (0-4096)
/// @param byte The byte that is written into memory
void virtual_machine_write_byte_to_memory(virtual_machine_t * vm, uint16_t * memoryLocation, uint8_t byte) {
    if (*memoryLocation > 0x1000u || *memoryLocation < PROGRAM_START_LOCATION) {
        return;
    }
    vm->memory[(*memoryLocation)++] = byte;
}

/// @brief Places sprites for characters in memory
/// @param vm The virtual machine where the sprites are placed in memory
static inline void virtual_machine_place_character_sprites_in_memory(virtual_machine_t * vm) {
    memcpy(vm->memory + 0x50, CHARACTER_SPRITES, 80);
}

/// Executes the next opcode in memory
/// @param vm The chip8 virtual machine where the next opcode is executed
/// @return 0 if the opcode was executed properly, -1 if not
static int8_t virtual_machine_execute_next_opcode(virtual_machine_t * vm, uint16_t keyBoardState) {
    switch (vm->currentOpcode & 0xf000) {
    case 0x0000:
        {
            switch (vm->currentOpcode & 0x0fff) {
            case 0x001: // 0x0001 - NOP
                break;
            case 0x002: // 0x0002 - EXT
                exit(0);
                break;
            case 0x0E0: // 0x00E0 - Clear the screen
                {
                    for (uint8_t x = 0; x < GRAPHICS_SYSTEM_WIDTH; x++) {
                        for (uint8_t y = 0; y < GRAPHICS_SYSTEM_HEIGHT; y++) {
                            vm->display.graphicsSystem[x][y] = 0;
                        }
                    }
                    break;
                }
            case 0x0E1: // 0x00E1 - Toggle the pixels on the screen
                {
                    for (uint8_t x = 0; x < GRAPHICS_SYSTEM_WIDTH; x++) {
                        for (uint8_t y = 0; y < GRAPHICS_SYSTEM_HEIGHT; y++) {
                            vm->display.graphicsSystem[x][y] = !vm->display.graphicsSystem[x][y];
                        }
                    }
                    break;
                }
            case 0x0EE: // 0x00EE - return from subroutine
                vm->programCounter = *--vm->stackPointer;
                break;
            default:
                goto chip8_error;
            }
            break;
        }
    case 0x1000: // 0x1NNN - Jumps to address NNN
        {
            vm->programCounter = (((vm->currentOpcode & 0x0fff) - PROGRAM_START_LOCATION) / 2) - 1;
            break;
        }
    case 0x2000: // 0x2NNN - Calls subroutine at NNN
        {
            DEFINE_12_BIT_VALUE
            *vm->stackPointer++ = vm->programCounter;
            vm->programCounter = value;
            break;
        }
    case 0x3000: // 0x3XNN - Skips the next instruction if VX equals NN. Usually the next instruction is a jump to skip
                 // a code block
        {
            DEFINE_8_BIT_VALUE
            DEFINE_X
            if (vm->V[x] == value) {
                vm->programCounter++;
            }
            break;
        }
    case 0x4000: // 0x4XNN - Skips the next instruction if VX does not equal NN. Usually the next instruction is a jump
                 // to skip a code block
        {
            DEFINE_8_BIT_VALUE
            DEFINE_X
            if (vm->V[x] != value) {
                vm->programCounter++;
            }
            break;
        }
    case 0x5000: // 0x5XY0 - Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip
                 // a code block)
        {
            if (vm->currentOpcode & 0x000f) {
                goto chip8_error;
            }
            DEFINE_X
            DEFINE_Y
            if (vm->V[x] == vm->V[y]) {
                vm->programCounter++;
            }
            break;
        }
    case 0x6000: // 0x6XNN - Sets VX to NN
        {
            DEFINE_8_BIT_VALUE
            DEFINE_X
            vm->V[x] = value;
            break;
        }
    case 0x7000: // 0x7XNN - Adds NN to VX. (Carry flag is not changed)
        {
            DEFINE_8_BIT_VALUE
            DEFINE_X
            vm->V[x] += value;
            break;
        }
    case 0x8000:
        switch (vm->currentOpcode & 0x000f) {
        case 0x0: // 0x8XY0 - Sets VX to the value of VY
            {
                DEFINE_X
                DEFINE_Y
                vm->V[x] = vm->V[y];
                break;
            }
        case 0x1: // 0x8XY1 - Sets VX to the value of VX or VY
            {
                DEFINE_X
                DEFINE_Y
                vm->V[x] |= vm->V[y];
                break;
            }
        case 0x2: // 0x8XY2 - Sets VX to the value of VX and VY
            {
                DEFINE_X
                DEFINE_Y
                vm->V[x] &= vm->V[y];
                break;
            }
        case 0x3: // 0x8XY3 - Sets VX to the value of VX xor VY
            {
                DEFINE_X
                DEFINE_Y
                vm->V[x] ^= vm->V[y];
                break;
            }
        case 0x4: // 0x8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not.
            {
                DEFINE_X
                DEFINE_Y
                uint8_t result = vm->V[x] + vm->V[y];
                if (result > (result - vm->V[x])) {
                    vm->V[0xf] = 0x1u;
                }
                vm->V[x] = result;
                break;
            }
        case 0x5: // 0x8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
            {
                DEFINE_X
                DEFINE_Y
                uint8_t result = vm->V[x] - vm->V[y];
                if (result < (result - vm->V[x])) {
                    vm->V[0xf] = 0x1u;
                }
                vm->V[x] = result;
                break;
            }
        case 0x6: // 0x8XY6 - Stores the least significant bit of VX in VF and then shifts VX to the right by 1
            {
                DEFINE_X
                vm->V[0xf] = vm->V[x] & 0x0001;
                vm->V[x] >>= 1;
                break;
            }
        case 0x7: // 0x8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
            {
                DEFINE_X
                DEFINE_Y
                uint8_t result = vm->V[y] - vm->V[x];
                if (result > (result - vm->V[x])) {
                    vm->V[0xf] = 0x1u;
                }
                vm->V[x] = result;
                break;
            }
        case 0xe: // 0x8XYe - Stores the most significant bit of VX in VF and then shifts VX to the left by 1
            {
                DEFINE_X
                vm->V[0xf] = vm->V[x] & 0x0001;
                vm->V[x] <<= 1;
                break;
            }
        default:
            goto chip8_error;
        }
        break;
    case 0x9000: // 0x9XY0 - Skips the next instruction if VX does not equal VY. Usually the next instruction is a jump
                 // to skip a code block
        {
            if (vm->currentOpcode & 0x000f) {
                goto chip8_error;
            }
            DEFINE_X
            DEFINE_Y
            if (vm->V[x] == vm->V[y]) {
                vm->programCounter++;
            }
            break;
        }
    case 0xa000: // 0xANNN - Sets I to the address NNN.
        {
            vm->I = vm->currentOpcode & 0x0fff;
            break;
        }
    case 0xb000: // 0xBNNN - Jumps to the address NNN plus V0
        {
            vm->programCounter = (vm->currentOpcode & 0x0fff + vm->V[0] / 2);
            break;
        }
    case 0xc000: // 0xCXNN - Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255)
                 // and NN.
        {
            DEFINE_8_BIT_VALUE
            DEFINE_X
            vm->V[x] = (rand() & 255) & value;
            break;
        }
    case 0xd000: /* 0xDXYN - Draws a sprite at coordinate (VX, VY)
                  * that has a width of 8 pixels and a height of N pixels.
                  * Each row of 8 pixels is read as bit-coded starting from memory location I;
                  * I value does not change after the execution of this instruction.
                  * As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the
                  * sprite is drawn, and to 0 if that does not happen
                  */
        {
            DEFINE_X
            DEFINE_Y
            uint8_t spriteHeight = vm->currentOpcode & 0x000f;
            bool setVF = false;
            // Stays zero if no screen pixels are flipped from set to unset
            vm->V[0xf] = 0;
            for (size_t height = 0; height < spriteHeight; height++) {
                for (size_t width = 0; width < 8; width++) {
                    if ((vm->memory[(vm->I + height) & 4095] >> (8 - width)) & 0x01) {
                        if (!setVF && vm->display.graphicsSystem[width + vm->V[x] & (GRAPHICS_SYSTEM_WIDTH - 1)]
                                                                [height + vm->V[y] & (GRAPHICS_SYSTEM_HEIGHT - 1)]) {
                            vm->V[0xf] = 1;
                            setVF = true;
                        }
                        vm->display.graphicsSystem[width + vm->V[x] & (GRAPHICS_SYSTEM_WIDTH - 1)]
                                                  [height + vm->V[y] & (GRAPHICS_SYSTEM_HEIGHT - 1)] =
                            !vm->display.graphicsSystem[width + vm->V[x] & (GRAPHICS_SYSTEM_WIDTH - 1)]
                                                       [height + vm->V[y] & (GRAPHICS_SYSTEM_HEIGHT - 1)];
                    }
                }
            }
            break;
        }
    case 0xe000:
        {
            switch (vm->currentOpcode & 0x00ff) {
            case 0x9e: // 0xEX9E - Skips the next instruction if the key stored in VX is pressed. (Usually the next
                       // instruction is a jump to skip a code block)
                {
                    DEFINE_X
                    switch (vm->V[x]) {
                    case 0x0:
                        if (keyBoardState & CHIP8_KEY_CODE_0) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x1:
                        if (keyBoardState & CHIP8_KEY_CODE_1) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x2:
                        if (keyBoardState & CHIP8_KEY_CODE_2) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x3:
                        if (keyBoardState & CHIP8_KEY_CODE_3) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x4:
                        if (keyBoardState & CHIP8_KEY_CODE_4) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x5:
                        if (keyBoardState & CHIP8_KEY_CODE_5) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x6:
                        if (keyBoardState & CHIP8_KEY_CODE_6) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x7:
                        if (keyBoardState & CHIP8_KEY_CODE_7) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x8:
                        if (keyBoardState & CHIP8_KEY_CODE_8) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x9:
                        if (keyBoardState & CHIP8_KEY_CODE_9) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xA:
                        if (keyBoardState & CHIP8_KEY_CODE_A) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xB:
                        if (keyBoardState & CHIP8_KEY_CODE_B) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xC:
                        if (keyBoardState & CHIP8_KEY_CODE_C) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xD:
                        if (keyBoardState & CHIP8_KEY_CODE_D) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xE:
                        if (keyBoardState & CHIP8_KEY_CODE_E) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xF:
                        if (keyBoardState & CHIP8_KEY_CODE_F) {
                            vm->programCounter++;
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                }
            case 0xa1: // 0xEXA1 - Skips the next instruction if the key stored in VX is not pressed. (Usually the next
                       // instruction is a jump to skip a code block)
                {
                    DEFINE_X
                    switch (vm->V[x]) {
                    case 0x0:
                        if (!(keyBoardState & CHIP8_KEY_CODE_0)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x1:
                        if (!(keyBoardState & CHIP8_KEY_CODE_1)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x2:
                        if (!(keyBoardState & CHIP8_KEY_CODE_2)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x3:
                        if (!(keyBoardState & CHIP8_KEY_CODE_3)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x4:
                        if (!(keyBoardState & CHIP8_KEY_CODE_4)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x5:
                        if (!(keyBoardState & CHIP8_KEY_CODE_5)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x6:
                        if (!(keyBoardState & CHIP8_KEY_CODE_6)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x7:
                        if (!(keyBoardState & CHIP8_KEY_CODE_7)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x8:
                        if (!(keyBoardState & CHIP8_KEY_CODE_8)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0x9:
                        if (!(keyBoardState & CHIP8_KEY_CODE_9)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xA:
                        if (!(keyBoardState & CHIP8_KEY_CODE_A)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xB:
                        if (!(keyBoardState & CHIP8_KEY_CODE_B)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xC:
                        if (!(keyBoardState & CHIP8_KEY_CODE_C)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xD:
                        if (!(keyBoardState & CHIP8_KEY_CODE_D)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xE:
                        if (!(keyBoardState & CHIP8_KEY_CODE_E)) {
                            vm->programCounter++;
                        }
                        break;
                    case 0xF:
                        if (!(keyBoardState & CHIP8_KEY_CODE_F)) {
                            vm->programCounter++;
                        }
                        break;
                    default:
                        break;
                    }
                    break;
                }
            default:
                goto chip8_error;
            }
            break;
        }
    case 0xf000:
        {
            switch (vm->currentOpcode & 0x00ff) {
            case 0x00: // 0xFX00 - Prints the character stored in the register VX
                {
                    DEFINE_X
                    putchar(vm->V[x]);
                    // We need to flush the buffer to make sure the character is printed
                    fflush(stdout);
                    break;
                }
            case 0x7: // 0xFX07 - Sets VX to the value of the delay timer.
                {
                    DEFINE_X
                    vm->V[x] = vm->delayTimer;
                    break;
                }
            case 0xa: // 0xFX0A - A key press is awaited, and then stored in VX. (Blocking Operation. All instruction
                      // halted until next key event)
                {
                    DEFINE_X
                    vm->V[x] = getchar();
                    break;
                }
            case 0x15: // 0xFX15 - Sets the delay timer to VX
                {
                    DEFINE_X
                    vm->delayTimer = vm->V[x];
                    break;
                }
            case 0x18: // 0xFX18 - Sets the sound timer to VX.
                {
                    DEFINE_X
                    vm->soundTimer = vm->V[x];
                    break;
                }
            case 0x1e: // 0xFX1E - Adds VX to I. VF is not affected
                {
                    DEFINE_X
                    vm->I += vm->V[x];
                    break;
                }
            case 0x29:
                // 0xFX29 - Sets I to the location of the sprite for the character in VX. Characters
                // are represented by a 4x5 font The characters are stored at the address 0x0050 and are 20 bit large (4
                // by 5 bits)
                {
                    DEFINE_X
                    if (vm->V[x] <= '9' && vm->V[x] >= '0') {
                        vm->I = 0x0050 + 0x5 * (vm->V[x] - '0');
                    } else if (vm->V[x] <= 'F' && vm->V[x] >= 'A') {
                        vm->I = 0x0050 + 0x5 * (vm->V[x] - 0x37);
                    } else {
                        goto chip8_error;
                    }
                    break;
                }
            case 0x33: /* 0xFX33 - Stores the binary-coded decimal representation of VX,
                        * with the most significant of three digits at the address in I,
                        * the middle digit at I plus 1, and the least significant digit at I plus 2.
                        * (In other words, take the decimal representation of VX, place the hundreds digit in memory at
                        * location in I, the tens digit at location I+1, and the ones digit at location I+2.)
                        */
                {
                    DEFINE_X
                    uint8_t value = vm->V[x];
                    uint8_t base = 100u;
                    for (uint8_t i = 0u; base; i++, value %= base, base /= 10) {
                        vm->memory[(vm->I + i) & 4095] = value / base;
                    }
                    break;
                }
            case 0x55: /* 0xFX55 - Stores from V0 to VX (including VX) in memory, starting at address I.
                        * The offset from I is increased by 1 for each value written, but I itself is left unmodified
                        */
                {
                    DEFINE_X
                    for (uint8_t i = 0u; i <= x; i++) {
                        vm->memory[(vm->I + i) & 4095] = vm->V[i];
                    }
                    break;
                }
            case 0x65: /* 0xFX65 - Fills from V0 to VX (including VX) with values from memory, starting at address I.
                        * The offset from I is increased by 1 for each value read, but I itself is left unmodified
                        */
                {
                    DEFINE_X
                    for (uint8_t i = 0u; i <= x; i++) {
                        vm->V[i] = vm->memory[(vm->I + i) & 4095];
                    }
                    break;
                }
            default:
                goto chip8_error;
            }
            break;
        }
    default:
        goto chip8_error;
    }
    return 0;

chip8_error:
    printf("Unknown opcode: 0x%4X", vm->currentOpcode);
    return -1;
}
