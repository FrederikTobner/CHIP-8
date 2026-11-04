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

/// The character sprites that are stored in memory (from 0x)
#define CHARACTER_SPRITES                                                                                           \
    ("\xF0\x90\x90\x90\xF0\x20\x60\x20\x20\x70\xF0\x10\xF0\x80\xF0\xF0\x10\xF0\x10\xF0\x90\x90\xF0\x10\x10\xF0\x80" \
     "\xF0\x10\xF0\xF0\x80\xF0\x90\xF0\xF0\x10\x20\x40\x40\xF0\x90\xF0\x90\xF0\xF0\x90\xF0\x10\xF0\xF0\x90\xF0\x90" \
     "\x90\xE0\x90\xE0\x90\xE0\xF0\x80\x80\x80\xF0\xE0\x90\x90\x90\xE0\xF0\x80\xF0\x80\xF0\xF0\x80\xF0\x80\x80")

static vm_status_t virtual_machine_execute_next_opcode(virtual_machine_t *, keyBoardState_t);
static bool virtual_machine_poll_events(virtual_machine_t *, keyBoardState_t *);
static inline void virtual_machine_place_character_sprites_in_memory(virtual_machine_t *);

/// @brief Executes the program that is stored in memory
/// @param vm The chip8 vm where the program that is currently held in memory is executed
void virtual_machine_execute(virtual_machine_t * vm) {
    keyBoardState_t keyBoardState = 0;
    uint64_t const frequency = SDL_GetPerformanceFrequency();
    uint64_t const ticksPerFrame = frequency / VM_FRAMES_PER_SECOND;
    uint64_t nextFrameTime = SDL_GetPerformanceCounter();
    while (virtual_machine_poll_events(vm, &keyBoardState)) {
        if (virtual_machine_run_frame(vm, keyBoardState) != VM_OK) {
            return;
        }
        // Only the frames where the program changed the display have to be rendered
        if (vm->display.dirty) {
            display_render(&vm->display);
        }
        // Waiting for the next frame - frames are scheduled based on the start of the previous one, so the delays of
        // a single frame do not accumulate
        nextFrameTime += ticksPerFrame;
        uint64_t const now = SDL_GetPerformanceCounter();
        if (now < nextFrameTime) {
            SDL_Delay((uint32_t)(((nextFrameTime - now) * 1000u) / frequency));
        } else if (now - nextFrameTime > ticksPerFrame) {
            // We are more than a frame behind (e.g. the window was dragged) - skip ahead instead of running faster
            nextFrameTime = now;
        }
    }
}

/// @brief Executes the instructions of a single frame (1/60 second) and ticks the timers afterwards
/// @param vm The chip8 virtual machine that executes the instructions
/// @param keyBoardState The keys that are currently pressed
/// @return VM_OK if the program can continue, VM_HALT if it is finished and VM_ERROR if the opcode is unknown
vm_status_t virtual_machine_run_frame(virtual_machine_t * vm, keyBoardState_t keyBoardState) {
    for (uint16_t instruction = 0; instruction < vm->instructionsPerFrame; instruction++) {
        vm_status_t status = virtual_machine_step(vm, keyBoardState);
        if (status != VM_OK) {
            return status;
        }
    }
    if (vm->delayTimer) {
        vm->delayTimer--;
    }
    if (vm->soundTimer) {
        putc('\a', stdout);
        vm->soundTimer--;
    }
    return VM_OK;
}

/// @brief Executes the next instruction of the program
/// @param vm The chip8 virtual machine that executes the instruction
/// @param keyBoardState The keys that are currently pressed
/// @return VM_OK if the program can continue, VM_HALT if it is finished and VM_ERROR if the opcode is unknown
vm_status_t virtual_machine_step(virtual_machine_t * vm, keyBoardState_t keyBoardState) {
    // Reached end of the memory (an instruction has 2 bytes and does not have to start at an even address)
    if (vm->programCounter > 0x0ffe) {
        return VM_HALT;
    }
#ifdef TRACE_EXECUTION
    debug_trace_execution(*vm);
#endif
    vm->currentOpcode = (uint16_t)vm->memory[vm->programCounter + 1];
    vm->currentOpcode += vm->memory[vm->programCounter] << 8;
    // Reached end of the program
    if (!vm->currentOpcode) {
        return VM_HALT;
    }
    // The program counter points to the next instruction while the current one is executed
    vm->programCounter += 2;
    return virtual_machine_execute_next_opcode(vm, keyBoardState);
}

/// @brief Sets the speed of the virtual machine
/// @param vm The chip8 virtual machine where the speed is set
/// @param instructionsPerSecond The amount of instructions that are executed per second (rounded to a whole amount of
/// instructions per frame, at least one instruction per frame)
void virtual_machine_set_speed(virtual_machine_t * vm, uint32_t instructionsPerSecond) {
    uint32_t instructionsPerFrame = (instructionsPerSecond + VM_FRAMES_PER_SECOND / 2) / VM_FRAMES_PER_SECOND;
    if (!instructionsPerFrame) {
        instructionsPerFrame = 1;
    }
    if (instructionsPerFrame > UINT16_MAX) {
        instructionsPerFrame = UINT16_MAX;
    }
    vm->instructionsPerFrame = (uint16_t)instructionsPerFrame;
}

/// @brief Polls the pending SDL events and updates the state of the keyboard
/// @param vm The chip8 virtual machine whose display is redrawn when the window needs it
/// @param keyBoardState The state of the keyboard that is updated
/// @return false if the window was closed, true otherwise
static bool virtual_machine_poll_events(virtual_machine_t * vm, keyBoardState_t * keyBoardState) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
        // The content of the window has to be rendered again, even if the program did not change the display
        if (event.type == SDL_WINDOWEVENT && (event.window.event == SDL_WINDOWEVENT_EXPOSED ||
                                              event.window.event == SDL_WINDOWEVENT_RESTORED ||
                                              event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
            vm->display.dirty = true;
        }
        keyboard_handle_event(event, keyBoardState);
    }
    return true;
}

/// @brief Initializes the chip8 vm
/// @param vm The chip8 virtual machine that is initialzed
void virtual_machine_init(virtual_machine_t * vm) {
    uint8_t * upperBound = (vm->V + 16u);
    for (uint8_t * memoryPointer = vm->V; memoryPointer < upperBound; memoryPointer++) {
        *memoryPointer = 0u;
    }
    vm->currentOpcode = 0u;
    vm->I = 0u;
    vm->delayTimer = 0u;
    vm->soundTimer = 0u;
    memset(vm->display.graphicsSystem, 0, sizeof(vm->display.graphicsSystem));
    vm->display.dirty = false;
    vm->waitingForKey = false;
    vm->keyWaitBaseline = 0u;
    virtual_machine_set_speed(vm, VM_DEFAULT_INSTRUCTIONS_PER_SECOND);
    // Initialize stackpointer
    vm->stackPointer = vm->stack;
    // Initialize program counter
    vm->programCounter = PROGRAM_START_LOCATION;
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
/// @return VM_OK if the opcode was executed properly, VM_HALT if the program is finished, VM_ERROR if the opcode is
/// unknown
static vm_status_t virtual_machine_execute_next_opcode(virtual_machine_t * vm, keyBoardState_t keyBoardState) {
    switch (vm->currentOpcode & 0xf000) {
    case 0x0000:
        {
            switch (vm->currentOpcode & 0x0fff) {
            case 0x001: // 0x0001 - NOP
                break;
            case 0x002: // 0x0002 - EXT
                return VM_HALT;
            case 0x0E0: // 0x00E0 - Clear the screen
                {
                    for (uint8_t x = 0; x < GRAPHICS_SYSTEM_WIDTH; x++) {
                        for (uint8_t y = 0; y < GRAPHICS_SYSTEM_HEIGHT; y++) {
                            vm->display.graphicsSystem[x][y] = 0;
                        }
                    }
                    vm->display.dirty = true;
                    break;
                }
            case 0x0E1: // 0x00E1 - Toggle the pixels on the screen
                {
                    for (uint8_t x = 0; x < GRAPHICS_SYSTEM_WIDTH; x++) {
                        for (uint8_t y = 0; y < GRAPHICS_SYSTEM_HEIGHT; y++) {
                            vm->display.graphicsSystem[x][y] = !vm->display.graphicsSystem[x][y];
                        }
                    }
                    vm->display.dirty = true;
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
            vm->programCounter = vm->currentOpcode & 0x0fff;
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
                vm->programCounter += 2;
            }
            break;
        }
    case 0x4000: // 0x4XNN - Skips the next instruction if VX does not equal NN. Usually the next instruction is a jump
                 // to skip a code block
        {
            DEFINE_8_BIT_VALUE
            DEFINE_X
            if (vm->V[x] != value) {
                vm->programCounter += 2;
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
                vm->programCounter += 2;
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
                uint16_t sum = vm->V[x] + vm->V[y];
                vm->V[x] = sum & 0xff;
                // The flag is written last, so it also is correct when VX is VF
                vm->V[0xf] = sum > 0xff;
                break;
            }
        case 0x5: // 0x8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.
            {
                DEFINE_X
                DEFINE_Y
                uint8_t noBorrow = vm->V[x] >= vm->V[y];
                vm->V[x] -= vm->V[y];
                vm->V[0xf] = noBorrow;
                break;
            }
        case 0x6: // 0x8XY6 - Stores the least significant bit of VX in VF and then shifts VX to the right by 1
            {
                DEFINE_X
                uint8_t leastSignificantBit = vm->V[x] & 0x01;
                vm->V[x] >>= 1;
                vm->V[0xf] = leastSignificantBit;
                break;
            }
        case 0x7: // 0x8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.
            {
                DEFINE_X
                DEFINE_Y
                uint8_t noBorrow = vm->V[y] >= vm->V[x];
                vm->V[x] = vm->V[y] - vm->V[x];
                vm->V[0xf] = noBorrow;
                break;
            }
        case 0xe: // 0x8XYe - Stores the most significant bit of VX in VF and then shifts VX to the left by 1
            {
                DEFINE_X
                uint8_t mostSignificantBit = (vm->V[x] >> 7) & 0x01;
                vm->V[x] <<= 1;
                vm->V[0xf] = mostSignificantBit;
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
            if (vm->V[x] != vm->V[y]) {
                vm->programCounter += 2;
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
            vm->programCounter = ((vm->currentOpcode & 0x0fff) + vm->V[0]) & 0x0fff;
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
            // Stays zero if no screen pixels are flipped from set to unset
            vm->V[0xf] = 0;
            for (size_t height = 0; height < spriteHeight; height++) {
                uint8_t spriteRow = vm->memory[(vm->I + height) & 4095];
                size_t row = (height + vm->V[y]) & (GRAPHICS_SYSTEM_HEIGHT - 1);
                for (size_t width = 0; width < 8; width++) {
                    // The most significant bit is the leftmost pixel of the sprite
                    if ((spriteRow >> (7 - width)) & 0x01) {
                        size_t column = (width + vm->V[x]) & (GRAPHICS_SYSTEM_WIDTH - 1);
                        if (vm->display.graphicsSystem[column][row]) {
                            vm->V[0xf] = 1;
                        }
                        vm->display.graphicsSystem[column][row] = !vm->display.graphicsSystem[column][row];
                        vm->display.dirty = true;
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
                    if (keyBoardState & (1u << (vm->V[x] & 0x0f))) {
                        vm->programCounter += 2;
                    }
                    break;
                }
            case 0xa1: // 0xEXA1 - Skips the next instruction if the key stored in VX is not pressed. (Usually the next
                       // instruction is a jump to skip a code block)
                {
                    DEFINE_X
                    if (!(keyBoardState & (1u << (vm->V[x] & 0x0f)))) {
                        vm->programCounter += 2;
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
                    if (!vm->waitingForKey) {
                        // Keys that are already held down when the instruction is reached do not count
                        vm->waitingForKey = true;
                        vm->keyWaitBaseline = keyBoardState;
                    }
                    // Keys that were released in the meantime can be pressed again
                    vm->keyWaitBaseline &= keyBoardState;
                    keyBoardState_t newlyPressedKeys = keyBoardState & ~vm->keyWaitBaseline;
                    if (newlyPressedKeys) {
                        uint8_t key = 0;
                        while (!(newlyPressedKeys & (1u << key))) {
                            key++;
                        }
                        vm->V[x] = key;
                        vm->waitingForKey = false;
                    } else {
                        // Executing the same instruction again
                        vm->programCounter -= 2;
                    }
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
                // 0xFX29 - Sets I to the location of the sprite for the character in VX (0x0-0xF). Characters are
                // represented by a 4x5 font. The characters are stored at the address 0x0050 and every character is 5
                // bytes large
                {
                    DEFINE_X
                    vm->I = 0x0050 + 0x5 * (vm->V[x] & 0x0f);
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
    return VM_OK;

chip8_error:
    fprintf(stderr, "Unknown opcode: 0x%04X at address 0x%03X\n", vm->currentOpcode,
            vm->programCounter - 2);
    return VM_ERROR;
}
