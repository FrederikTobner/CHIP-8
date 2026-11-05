#include <gtest/gtest.h>

#include <string.h>

extern "C" {
#include "display.h"
}

namespace {

constexpr int kWidth = GRAPHICS_SYSTEM_WIDTH * SCALE_FACTOR;
constexpr int kHeight = GRAPHICS_SYSTEM_HEIGHT * SCALE_FACTOR;

/// Renders the display on a software surface, so no window (and no video driver) is needed
class DisplayRenderTest : public ::testing::Test {
  protected:
    void SetUp() override {
        SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
        surface = SDL_CreateRGBSurfaceWithFormat(0, kWidth, kHeight, 32, SDL_PIXELFORMAT_ARGB8888);
        ASSERT_NE(nullptr, surface) << SDL_GetError();
        memset(&display, 0, sizeof(display));
        display.renderer = SDL_CreateSoftwareRenderer(surface);
        ASSERT_NE(nullptr, display.renderer) << SDL_GetError();
        display.texture = SDL_CreateTexture(display.renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                            GRAPHICS_SYSTEM_WIDTH, GRAPHICS_SYSTEM_HEIGHT);
        ASSERT_NE(nullptr, display.texture) << SDL_GetError();
    }

    void TearDown() override {
        SDL_DestroyTexture(display.texture);
        SDL_DestroyRenderer(display.renderer);
        SDL_FreeSurface(surface);
    }

    /// Reads a pixel of the rendered image (0xAARRGGBB)
    uint32_t Pixel(int x, int y) {
        uint32_t pixel = 0;
        SDL_Rect rect = {x, y, 1, 1};
        SDL_RenderReadPixels(display.renderer, &rect, SDL_PIXELFORMAT_ARGB8888, &pixel, sizeof(pixel));
        return pixel;
    }

    SDL_Surface * surface = nullptr;
    display_t display;
};

TEST_F(DisplayRenderTest, EmptyDisplayIsWhite) {
    display_render(&display);
    EXPECT_EQ(0xFFFFFFFFu, Pixel(0, 0));
    EXPECT_EQ(0xFFFFFFFFu, Pixel(kWidth - 1, kHeight - 1));
}

TEST_F(DisplayRenderTest, SetPixelIsAScaledBlackSquare) {
    display.graphicsSystem[3][2] = 1;
    display_render(&display);
    // The pixel covers exactly SCALE_FACTOR x SCALE_FACTOR pixels without blurred edges
    for (int x : {3 * SCALE_FACTOR, 3 * SCALE_FACTOR + SCALE_FACTOR - 1}) {
        for (int y : {2 * SCALE_FACTOR, 2 * SCALE_FACTOR + SCALE_FACTOR - 1}) {
            EXPECT_EQ(0xFF000000u, Pixel(x, y)) << x << "/" << y;
        }
    }
    EXPECT_EQ(0xFFFFFFFFu, Pixel(3 * SCALE_FACTOR - 1, 2 * SCALE_FACTOR));
    EXPECT_EQ(0xFFFFFFFFu, Pixel(4 * SCALE_FACTOR, 2 * SCALE_FACTOR));
    EXPECT_EQ(0xFFFFFFFFu, Pixel(3 * SCALE_FACTOR, 2 * SCALE_FACTOR - 1));
    EXPECT_EQ(0xFFFFFFFFu, Pixel(3 * SCALE_FACTOR, 3 * SCALE_FACTOR));
}

TEST_F(DisplayRenderTest, CornersAreRenderedAtTheRightPlace) {
    display.graphicsSystem[0][0] = 1;
    display.graphicsSystem[GRAPHICS_SYSTEM_WIDTH - 1][GRAPHICS_SYSTEM_HEIGHT - 1] = 1;
    display_render(&display);
    EXPECT_EQ(0xFF000000u, Pixel(0, 0));
    EXPECT_EQ(0xFF000000u, Pixel(kWidth - 1, kHeight - 1));
    EXPECT_EQ(0xFFFFFFFFu, Pixel(kWidth - 1, 0));
    EXPECT_EQ(0xFFFFFFFFu, Pixel(0, kHeight - 1));
}

TEST_F(DisplayRenderTest, RenderingMarksTheDisplayAsUpToDate) {
    display.dirty = true;
    display_render(&display);
    EXPECT_FALSE(display.dirty);
}

} // namespace
