#include <gtest/gtest.h>

#include <fstream>
#include <iterator>
#include <string.h>
#include <string>
#include <vector>

#include <SDL.h>

// The generated header that embeds the icon in the backend library
#include "window_icon.h"

namespace {

TEST(WindowIconTest, EmbeddedIconIsIdenticalToTheFileInTheAssetsFolder) {
    std::ifstream file(std::string(CP8_ASSETS_DIR) + "/chip8_window_icon.bmp", std::ios::binary);
    ASSERT_TRUE(file.is_open());
    std::vector<char> expected((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    ASSERT_EQ(expected.size(), sizeof(WINDOW_ICON_DATA));
    EXPECT_EQ(0, memcmp(expected.data(), WINDOW_ICON_DATA, expected.size()));
}

TEST(WindowIconTest, EmbeddedIconCanBeLoadedAsBitmap) {
    SDL_RWops * source = SDL_RWFromConstMem(WINDOW_ICON_DATA, static_cast<int>(sizeof(WINDOW_ICON_DATA)));
    ASSERT_NE(nullptr, source) << SDL_GetError();
    // Loading a bitmap does not need an initialized video subsystem
    SDL_Surface * icon = SDL_LoadBMP_RW(source, 1);
    ASSERT_NE(nullptr, icon) << SDL_GetError();
    EXPECT_EQ(50, icon->w);
    EXPECT_EQ(50, icon->h);
    SDL_FreeSurface(icon);
}

} // namespace
