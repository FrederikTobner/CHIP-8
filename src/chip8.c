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

#include "chip8.h"

#ifdef OS_WINDOWS
    // Windows specific libary - conio.h (under linux curses.h could be used) 😟
    #include <conio.h>
#endif
// TODO: Use curses.h instead of conio.h for unix based systems


#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "debug.h"
#include "flags.h"

/// Defines a new 8-bit value based on the opcode that is currently executed
#define DEFINE_8_BIT_VALUE                      \
uint8_t value = chip8->currentOpcode & 0x00ff;

/// Defines a new 12-bit value based on the opcode that is currently executed
#define DEFINE_12_BIT_VALUE                     \
uint16_t value = chip8->currentOpcode & 0x0fff;


/// Defines a new 4-bit value based on the opcode that is currently executed
#define DEFINE_X                                \
uint8_t x = (chip8->currentOpcode & 0x0f00u) >> 8;

/// Defines a new 4-bit value based on the opcode that is currently executed
#define DEFINE_Y                                \
uint8_t y = (chip8->currentOpcode & 0x00f0u) >> 4;

static int8_t chip8_execute_next_opcode(chip8_t *);

/// @brief Executes the program that is stored in memory
/// @param chip8 The chip8 vm where the program that is currently held in memory is executed
void chip8_execute(chip8_t * chip8) 
{
    clock_t last_t, current_t;
    last_t = clock() / CLOCKS_PER_SEC;
    while (chip8->programCounter < 2048)
    {
        chip8->currentOpcode = 0;
        chip8->currentOpcode = chip8->memory[chip8->programCounter * 2];
        chip8->currentOpcode += chip8->memory[chip8->programCounter * 2 + 1] * 256;
        if(!chip8->currentOpcode)
            return;
        if(chip8_execute_next_opcode(chip8)) 
            return;
        current_t = clock() / CLOCKS_PER_SEC;
        long numberOfChip8Clocks = (current_t - last_t) / CLOCKS_PER_SEC * 60;
        if(numberOfChip8Clocks > 1) 
        {
            // Compute offset based on delay of the current time
            long offset = (current_t - last_t) % (CLOCKS_PER_SEC * 60);
            if(!chip8->delay_timer)
            {
                if(chip8->delay_timer < numberOfChip8Clocks)
                    chip8->delay_timer = 0;
                else
                chip8->delay_timer -= numberOfChip8Clocks;
            }
            if(!chip8->sound_timer)
            {
                putc('\a', stdout);
                if(chip8->sound_timer < numberOfChip8Clocks)
                    chip8->sound_timer = 0;
                else
                    chip8->sound_timer -= numberOfChip8Clocks;
            }
            last_t = current_t;
            // Adjust last measured time with the offset
            last_t -= offset;
        }
        chip8->programCounter++;        
    }
}

/// @brief Initializes the chip8 vm
/// @param chip8 The chip8 virtual machine that is initialzed
void chip8_init(chip8_t * chip8)
{
     // Initialize stackpointer
    chip8->stackPointer = chip8->stack;
    // Initialize program counter
    chip8->programCounter = 0u;
    // Compute upper bound for memory loop
    uint8_t * upperBound = (chip8->memory + 4096);
    // Initialize memory
    for (uint8_t * memoryPointer = chip8->memory; memoryPointer < upperBound; memoryPointer++)
        *memoryPointer = 0u;
}


/// @brief Writtes the specified opcode at the specified location into memory
/// @param chip8 The chip8 where the opcode is written to memory
/// @param memoryLocation The location where the opcode is written to (0-4096)
/// @param opcode The opcode that is written into memory
void chip8_write_opcode_to_memory(chip8_t * chip8, uint16_t * memoryLocation, uint16_t opcode)
{
    #ifdef PRINT_BYTE_CODE
        debug_print_bytecode(*memoryLocation, opcode);
    #endif
    chip8->memory[(*memoryLocation)++] = opcode & 0x00ff;
    chip8->memory[(*memoryLocation)++] = (opcode & 0xff00) >> 8;
}

/// Executes the next opcode in memory
/// @param chip8 The chip8 virtual machine where the next opcode is executed
/// @return 0 if the opcode was executed properly, -1 if not
static int8_t chip8_execute_next_opcode(chip8_t * chip8)
{
    switch (chip8->currentOpcode & 0xf000)
    {
    case 0x0000:
    {
        switch (chip8->currentOpcode & 0x0fff)
        {
            case 0x0E0 : // 0x00E0 - Clear the screen
            {    
                for (uint8_t i = 0; i < 64; i++)
                    for (uint8_t j = 0; i < 32; j++)
                        chip8->graphicsSystem[i][j] = 0;
                break;
            }
            case 0x0EE: // 0x00EE - return from subroutine        
                chip8->programCounter = *--chip8->stackPointer;
                break;    
            default:
                goto chip8_error;
        }
        break;
    }    
    case 0x1000: // 0x1NNN - Jumps to address NNN    
    {   
        chip8->programCounter = chip8->currentOpcode & 0x0fff;
        break;
    }
    case 0x2000: // 0x2NNN - Calls subroutine at NNN
    { 
        DEFINE_12_BIT_VALUE
        *chip8->stackPointer++ = chip8->programCounter;
        chip8->programCounter = value;
        break;
    }
    case 0x3000: // 0x3XNN - Skips the next instruction if VX equals NN. Usually the next instruction is a jump to skip a code block
    {   
        DEFINE_8_BIT_VALUE
        DEFINE_X
        if(chip8->V[x] == value)
            chip8->programCounter++;
        break;
    }
    case 0x4000: // 0x4XNN - Skips the next instruction if VX does not equal NN. Usually the next instruction is a jump to skip a code block
    {   
        DEFINE_8_BIT_VALUE
        DEFINE_X
        if(chip8->V[x] != value)
            chip8->programCounter++;
        break;
    }
    case 0x5000: // 0x5XY0 - Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
    {
        if(chip8->currentOpcode  & 0x000f)
           goto chip8_error;
        DEFINE_X
        DEFINE_Y
        if(chip8->V[x] == chip8->V[y])
            chip8->programCounter++;
        break;
    }
    case 0x6000: // 0x6XNN - Sets VX to NN
    {   
        DEFINE_8_BIT_VALUE
        DEFINE_X
        chip8->V[x] = value;
        break;
    }
    case 0x7000: // 0x7XNN - Adds NN to VX. (Carry flag is not changed)
    {
        DEFINE_8_BIT_VALUE
        DEFINE_X
        chip8->V[x] = value;
        break;
    }
    case 0x8000: 
        switch (chip8->currentOpcode & 0x000f)
        {
        case 0x0: // 0x8XY0 - Sets VX to the value of VY
        {    
            DEFINE_X
            DEFINE_Y
            chip8->V[x] = chip8->V[y];
            break;
        }
        case 0x1: // 0x8XY1 - Sets VX to the value of VX or VY
        {   
            DEFINE_X
            DEFINE_Y
            chip8->V[x] |= chip8->V[y];
            break;
        }
        case 0x2: // 0x8XY2 - Sets VX to the value of VX and VY
        {   
            DEFINE_X
            DEFINE_Y
            chip8->V[x] &= chip8->V[y];
            break;
        }
        case 0x3: // 0x8XY3 - Sets VX to the value of VX xor VY
        {   
            DEFINE_X
            DEFINE_Y
            chip8->V[x] ^= chip8->V[y];
            break;
        }
        case 0x4: // 0x8XY4 - Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there is not. 
        {   
            DEFINE_X
            DEFINE_Y
            uint8_t result = chip8->V[y] - chip8->V[x];
            if (result > (result-chip8->V[x]))
                chip8->V[0xf] = 0x1u;
            break;
        }
        case 0x5: // 0x8XY5 - VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there is not.  
        {   
            DEFINE_X
            DEFINE_Y
            uint8_t result = chip8->V[y] + chip8->V[x];
            if (result < (result-chip8->V[x]))
                chip8->V[0xf] = 0x1u;
            break;
        }
        case 0x6: // 0x8XY6 - Stores the least significant bit of VX in VF and then shifts VX to the right by 1
        {   
            DEFINE_X
            chip8->V[0xf] = chip8->V[x] & 0x0001;
            chip8->V[x] >>= 1;
            break;
        }
        case 0x7: // 0x8XY7 - Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there is not.  
        {   
            DEFINE_X
            DEFINE_Y
            uint8_t result = chip8->V[y] - chip8->V[x];
            if (result > (result - chip8->V[x]))
                chip8->V[0xf] = 0x1u;
            chip8->V[x] = result;
            break;
        }
        case 0xe: // 0x8XYe - Stores the most significant bit of VX in VF and then shifts VX to the left by 1
        {   
            DEFINE_X
            chip8->V[0xf] = chip8->V[x] & 0x0001;
            chip8->V[x] <<= 1;
            break;
        }        
        default:
            goto chip8_error;
        }
        break;
    case 0x9000: // 0x9XY0 - Skips the next instruction if VX does not equal VY. Usually the next instruction is a jump to skip a code block
    {
        if(chip8->currentOpcode & 0x000f)
            goto chip8_error;        
        DEFINE_X
        DEFINE_Y
        if(chip8->V[x] == chip8->V[y])
            chip8->programCounter++;
        break;
    }
    case 0xa000: // 0xANNN - Sets I to the address NNN.
    {    
        chip8->I = chip8->currentOpcode & 0x0fff;
        break;
    }
    case 0xb000: // 0xBNNN - Jumps to the address NNN plus V0
    {   
        chip8->programCounter = (chip8->currentOpcode & 0x0fff + chip8->V[0] / 2);
        break;
    }
    case 0xc000: // 0xCXNN - Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
    {
        DEFINE_8_BIT_VALUE
        DEFINE_X
        chip8->V[x] = rand() % 256 & value;
        break;        
    }
    case 0xd000: /* 0xDXYN - Draws a sprite at coordinate (VX, VY) 
    * that has a width of 8 pixels and a height of N pixels. 
    * Each row of 8 pixels is read as bit-coded starting from memory location I; 
    * I value does not change after the execution of this instruction. 
    * As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, 
    * and to 0 if that does not happen 
    */
        break;
    case 0xe000:
    {
        switch (chip8->currentOpcode & 0x00ff)
        {
        case 0x9e: // 0xEX9E - Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)    
        {   
            DEFINE_X
            // TODO: Add support for the instruction under unix based systems 
            #ifdef OS_WINDOWS
            if (kbhit())
            {
                char c = getch();
                if(chip8->V[x] == c)
                    chip8->programCounter++;
            }
            #endif
            break;
        }
        case 0xa1: // 0xEXA1 - Skips the next instruction if the key stored in VX is not pressed. (Usually the next instruction is a jump to skip a code block)         
        {   
            DEFINE_X
            // TODO: Add support for the instruction under unix based systems
            #ifdef OS_WINDOWS
            if (kbhit())
            {
                char c = getch();
                if(chip8->V[x] != c)
                    chip8->programCounter++;
            }
            else
                chip8->programCounter++;
            #endif    
            break;
        }
        default:
            goto chip8_error;
        }            
        break;
    }
    case 0xf000:
    {
        switch (chip8->currentOpcode & 0x00ff)
        {
        case 0x00: // 0xFX00 - Prints the character stored in the register VX
        {
            DEFINE_X
            putchar(chip8->V[x]);
            break;
        }
        case 0x7: // 0xFX07 - Sets VX to the value of the delay timer.
        {   
            DEFINE_X
            chip8->V[x] = chip8->delay_timer;
            break;
        }
        case 0xa: // 0xFX0A - A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
        {   
            DEFINE_X
            chip8->V[x] = getchar();
            break;
        }
        case 0x15: // 0xFX15 - Sets the delay timer to VX
        {   
            DEFINE_X
            chip8->delay_timer = chip8->V[x];
            break;
        }
        case 0x18: // 0xFX18 - Sets the sound timer to VX. 
        {   
            DEFINE_X
            chip8->sound_timer = chip8->V[x];
            break;
        }
        case 0x1e: // 0xFX1E - Adds VX to I. VF is not affected
        {   
            DEFINE_X
            chip8->I += chip8->V[x];
            break;
        }
        case 0x29: // 0xFX29 - Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font
            break;
        case 0x33: /* 0xFX33 - Stores the binary-coded decimal representation of VX, 
        * with the most significant of three digits at the address in I, 
        * the middle digit at I plus 1, and the least significant digit at I plus 2. 
        * (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I,
        * the tens digit at location I+1, and the ones digit at location I+2.) 
        */
        {
            DEFINE_X
            uint8_t value = chip8->V[x];
            uint8_t base = 100;
            for(uint32_t i = 0;;i++)
            {
                chip8->memory[chip8->I + i % 4096] = value / base;
                if(base == 1 )
                    break;
                value %= base;
                base /= 10;
            }        
            break;
        }            
        case 0x55: /* 0xFX55 - Stores from V0 to VX (including VX) in memory, starting at address I. 
        * The offset from I is increased by 1 for each value written, but I itself is left unmodified
        */
        {
            DEFINE_X
            for (uint8_t i = 0; i <= x; i++)
                chip8->memory[chip8->I + i % 4096] = chip8->V[i];     
            break;        
        }
        case 0x65: /* 0xFX65 - Fills from V0 to VX (including VX) with values from memory, starting at address I. 
        * The offset from I is increased by 1 for each value read, but I itself is left unmodified
        */
       {
            DEFINE_X
            for (uint8_t i = 0; i <= x; i++)
                chip8->V[i] = chip8->memory[chip8->I + i % 4096];          
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
    printf("Unknown opcode: 0x%4X", chip8->currentOpcode);
    return -1;
}
