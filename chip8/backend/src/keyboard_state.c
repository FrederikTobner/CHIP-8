#include "keyboard_state.h"

void keyboard_update_keyboard_down_state(SDL_Event event, keyBoardState_t * state) {
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

void keyboard_update_keyboard_up_state(SDL_Event event, keyBoardState_t * state) {
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