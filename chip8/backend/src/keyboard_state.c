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
 * @file keyboard_state.c
 * @brief Definitions regarding the keyboard of the emulator
 */

#include "keyboard_state.h"

void keyboard_handle_key_down_event(SDL_Event event, keyBoardState_t * state) {
    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_0:
        *state |= CHIP8_KEY_CODE_0;
        break;
    case SDL_SCANCODE_1:
        *state |= CHIP8_KEY_CODE_1;
        break;
    case SDL_SCANCODE_2:
        *state |= CHIP8_KEY_CODE_2;
        break;
    case SDL_SCANCODE_3:
        *state |= CHIP8_KEY_CODE_3;
        break;
    case SDL_SCANCODE_4:
        *state |= CHIP8_KEY_CODE_4;
        break;
    case SDL_SCANCODE_5:
        *state |= CHIP8_KEY_CODE_5;
        break;
    case SDL_SCANCODE_6:
        *state |= CHIP8_KEY_CODE_6;
        break;
    case SDL_SCANCODE_7:
        *state |= CHIP8_KEY_CODE_7;
        break;
    case SDL_SCANCODE_8:
        *state |= CHIP8_KEY_CODE_8;
        break;
    case SDL_SCANCODE_9:
        *state |= CHIP8_KEY_CODE_9;
        break;
    case SDL_SCANCODE_A:
        *state |= CHIP8_KEY_CODE_A;
        break;
    case SDL_SCANCODE_B:
        *state |= CHIP8_KEY_CODE_B;
        break;
    case SDL_SCANCODE_C:
        *state |= CHIP8_KEY_CODE_C;
        break;
    case SDL_SCANCODE_D:
        *state |= CHIP8_KEY_CODE_D;
        break;
    case SDL_SCANCODE_E:
        *state |= CHIP8_KEY_CODE_E;
        break;
    case SDL_SCANCODE_F:
        *state |= CHIP8_KEY_CODE_F;
        break;
    default:
        break;
    }
}

void keyboard_handle_key_up_event(SDL_Event event, keyBoardState_t * state) {
    switch (event.key.keysym.scancode) {
    case SDL_SCANCODE_0:
        *state ^= CHIP8_KEY_CODE_0;
        break;
    case SDL_SCANCODE_1:
        *state ^= CHIP8_KEY_CODE_1;
        break;
    case SDL_SCANCODE_2:
        *state ^= CHIP8_KEY_CODE_2;
        break;
    case SDL_SCANCODE_3:
        *state ^= CHIP8_KEY_CODE_3;
        break;
    case SDL_SCANCODE_4:
        *state ^= CHIP8_KEY_CODE_4;
        break;
    case SDL_SCANCODE_5:
        *state ^= CHIP8_KEY_CODE_5;
        break;
    case SDL_SCANCODE_6:
        *state ^= CHIP8_KEY_CODE_6;
        break;
    case SDL_SCANCODE_7:
        *state ^= CHIP8_KEY_CODE_7;
        break;
    case SDL_SCANCODE_8:
        *state ^= CHIP8_KEY_CODE_8;
        break;
    case SDL_SCANCODE_9:
        *state ^= CHIP8_KEY_CODE_9;
        break;
    case SDL_SCANCODE_A:
        *state ^= CHIP8_KEY_CODE_A;
        break;
    case SDL_SCANCODE_B:
        *state ^= CHIP8_KEY_CODE_B;
        break;
    case SDL_SCANCODE_C:
        *state ^= CHIP8_KEY_CODE_C;
        break;
    case SDL_SCANCODE_D:
        *state ^= CHIP8_KEY_CODE_D;
        break;
    case SDL_SCANCODE_E:
        *state ^= CHIP8_KEY_CODE_E;
        break;
    case SDL_SCANCODE_F:
        *state ^= CHIP8_KEY_CODE_F;
        break;
    default:
        break;
    }
}