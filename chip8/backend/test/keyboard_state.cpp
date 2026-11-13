#include <gtest/gtest.h>

#include <string.h>

extern "C" {
#include "backend/keyboard_state.h"
}
namespace {

SDL_Event KeyEvent(Uint32 type, SDL_Scancode scancode, bool repeat = false) {
    SDL_Event event;
    memset(&event, 0, sizeof(event));
    event.type = type;
    event.key.keysym.scancode = scancode;
    event.key.repeat = repeat ? 1 : 0;
    return event;
}

SDL_Event FocusEvent(Uint8 windowEvent) {
    SDL_Event event;
    memset(&event, 0, sizeof(event));
    event.type = SDL_WINDOWEVENT;
    event.window.event = windowEvent;
    return event;
}

TEST(Keyboard, KeyDownSetsAndKeyUpClearsTheKey) {
    keyBoardState_t state = 0;
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_4), &state);
    EXPECT_EQ(CHIP8_KEY_CODE_4, state);
    keyboard_handle_event(KeyEvent(SDL_KEYUP, SDL_SCANCODE_4), &state);
    EXPECT_EQ(0, state);
}

TEST(Keyboard, KeyUpWithoutKeyDownDoesNotSetTheKey) {
    keyBoardState_t state = 0;
    keyboard_handle_event(KeyEvent(SDL_KEYUP, SDL_SCANCODE_6), &state);
    EXPECT_EQ(0, state);
}

TEST(Keyboard, KeyUpOnlyClearsItsOwnKey) {
    keyBoardState_t state = 0;
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_4), &state);
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_6), &state);
    EXPECT_EQ(CHIP8_KEY_CODE_4 | CHIP8_KEY_CODE_6, state);
    keyboard_handle_event(KeyEvent(SDL_KEYUP, SDL_SCANCODE_4), &state);
    EXPECT_EQ(CHIP8_KEY_CODE_6, state);
}

TEST(Keyboard, RepeatedKeyDownEventsDoNotChangeTheState) {
    keyBoardState_t state = 0;
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_4), &state);
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_4, true), &state);
    EXPECT_EQ(CHIP8_KEY_CODE_4, state);
    keyboard_handle_event(KeyEvent(SDL_KEYUP, SDL_SCANCODE_4), &state);
    EXPECT_EQ(0, state);
}

TEST(Keyboard, RepeatedKeyDownEventDoesNotSetAKeyThatWasReleased) {
    keyBoardState_t state = 0;
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_4, true), &state);
    EXPECT_EQ(0, state);
}

TEST(Keyboard, AllHexadecimalDigitsAreMapped) {
    struct {
        SDL_Scancode scancode;
        int key;
    } const mapping[] = {
        {SDL_SCANCODE_0, 0x0}, {SDL_SCANCODE_1, 0x1}, {SDL_SCANCODE_2, 0x2}, {SDL_SCANCODE_3, 0x3},
        {SDL_SCANCODE_4, 0x4}, {SDL_SCANCODE_5, 0x5}, {SDL_SCANCODE_6, 0x6}, {SDL_SCANCODE_7, 0x7},
        {SDL_SCANCODE_8, 0x8}, {SDL_SCANCODE_9, 0x9}, {SDL_SCANCODE_A, 0xA}, {SDL_SCANCODE_B, 0xB},
        {SDL_SCANCODE_C, 0xC}, {SDL_SCANCODE_D, 0xD}, {SDL_SCANCODE_E, 0xE}, {SDL_SCANCODE_F, 0xF},
    };
    for (auto const & entry : mapping) {
        keyBoardState_t state = 0;
        keyboard_handle_event(KeyEvent(SDL_KEYDOWN, entry.scancode), &state);
        EXPECT_EQ(1u << entry.key, state) << entry.key;
    }
}

TEST(Keyboard, NumpadKeysAreAliasesOfTheDigits) {
    struct {
        SDL_Scancode scancode;
        int key;
    } const mapping[] = {
        {SDL_SCANCODE_KP_0, 0}, {SDL_SCANCODE_KP_1, 1}, {SDL_SCANCODE_KP_2, 2}, {SDL_SCANCODE_KP_3, 3},
        {SDL_SCANCODE_KP_4, 4}, {SDL_SCANCODE_KP_5, 5}, {SDL_SCANCODE_KP_6, 6}, {SDL_SCANCODE_KP_7, 7},
        {SDL_SCANCODE_KP_8, 8}, {SDL_SCANCODE_KP_9, 9},
    };
    for (auto const & entry : mapping) {
        keyBoardState_t state = 0;
        keyboard_handle_event(KeyEvent(SDL_KEYDOWN, entry.scancode), &state);
        EXPECT_EQ(1u << entry.key, state) << entry.key;
    }
}

TEST(Keyboard, UnmappedKeysAreIgnored) {
    keyBoardState_t state = 0;
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_Q), &state);
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_SPACE), &state);
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_G), &state);
    EXPECT_EQ(0, state);
}

TEST(Keyboard, KeysAreReleasedWhenTheWindowLosesTheFocus) {
    keyBoardState_t state = 0;
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_4), &state);
    keyboard_handle_event(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_6), &state);
    keyboard_handle_event(FocusEvent(SDL_WINDOWEVENT_FOCUS_GAINED), &state);
    EXPECT_NE(0, state);
    keyboard_handle_event(FocusEvent(SDL_WINDOWEVENT_FOCUS_LOST), &state);
    EXPECT_EQ(0, state);
}

} // namespace
