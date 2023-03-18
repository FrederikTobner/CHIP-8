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
 * @file keyboard_state.h
 * @brief Declarations regarding the keyboard of the emulator
 */

#ifndef CHIP8_KEY_BOARD_STATE_H_
#define CHIP8_KEY_BOARD_STATE_H_

#include "../../external/SDL/include/SDL.h"
#include "pre_compiled_header.h"

typedef uint16_t keyBoardState_t;

typedef enum {
    CHIP8_KEY_CODE_0 = 0b0000000000000001,
    CHIP8_KEY_CODE_1 = 0b0000000000000010,
    CHIP8_KEY_CODE_2 = 0b0000000000000100,
    CHIP8_KEY_CODE_3 = 0b0000000000001000,
    CHIP8_KEY_CODE_4 = 0b0000000000010000,
    CHIP8_KEY_CODE_5 = 0b0000000000100000,
    CHIP8_KEY_CODE_6 = 0b0000000001000000,
    CHIP8_KEY_CODE_7 = 0b0000000010000000,
    CHIP8_KEY_CODE_8 = 0b0000000100000000,
    CHIP8_KEY_CODE_9 = 0b0000001000000000,
    CHIP8_KEY_CODE_A = 0b0000010000000000,
    CHIP8_KEY_CODE_B = 0b0000100000000000,
    CHIP8_KEY_CODE_C = 0b0001000000000000,
    CHIP8_KEY_CODE_D = 0b0010000000000000,
    CHIP8_KEY_CODE_E = 0b0100000000000000,
    CHIP8_KEY_CODE_F = 0b1000000000000000,
} key_code;

void keyboard_update_keyboard_down_state(SDL_Event event, keyBoardState_t * state);

void keyboard_update_keyboard_up_state(SDL_Event event, keyBoardState_t * state);

#endif