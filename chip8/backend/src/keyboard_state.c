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

static keyBoardState_t keyboard_scancode_to_key_code(SDL_Scancode);

void keyboard_handle_event(SDL_Event event, keyBoardState_t * state) {
    switch (event.type) {
    case SDL_KEYDOWN:
        keyboard_handle_key_down_event(event, state);
        break;
    case SDL_KEYUP:
        keyboard_handle_key_up_event(event, state);
        break;
    case SDL_WINDOWEVENT:
        // We do not receive the key up events when the window is not focused, so the keys would be stuck
        if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            *state = 0;
        }
        break;
    default:
        break;
    }
}

void keyboard_handle_key_down_event(SDL_Event event, keyBoardState_t * state) {
    // The operating system repeats the key down event when a key is held down - the key is already pressed
    if (event.key.repeat) {
        return;
    }
    *state |= keyboard_scancode_to_key_code(event.key.keysym.scancode);
}

void keyboard_handle_key_up_event(SDL_Event event, keyBoardState_t * state) {
    *state &= ~keyboard_scancode_to_key_code(event.key.keysym.scancode);
}

/// @brief Converts a key of the keyboard to the key code of the CHIP-8 keyboard
/// @details The keys 0-9 (number row and numpad) and A-F are mapped to the corresponding CHIP-8 key
/// @param scancode The scancode of the key on the keyboard
/// @return The key code of the CHIP-8 key, 0 if the key is not mapped
static keyBoardState_t keyboard_scancode_to_key_code(SDL_Scancode scancode) {
    int key;
    if (scancode >= SDL_SCANCODE_A && scancode <= SDL_SCANCODE_F) {
        key = 0xA + (scancode - SDL_SCANCODE_A);
    } else if (scancode >= SDL_SCANCODE_1 && scancode <= SDL_SCANCODE_9) {
        key = 1 + (scancode - SDL_SCANCODE_1);
    } else if (scancode >= SDL_SCANCODE_KP_1 && scancode <= SDL_SCANCODE_KP_9) {
        key = 1 + (scancode - SDL_SCANCODE_KP_1);
    } else if (scancode == SDL_SCANCODE_0 || scancode == SDL_SCANCODE_KP_0) {
        key = 0;
    } else {
        return 0;
    }
    return (keyBoardState_t)(1u << key);
}
