#include <gtest/gtest.h>

#include <stdint.h>

#include "../src/fnv1a.h"

TEST(FNV1A, test) {
    uint32_t expectedHash = 2949673445;
    size_t s = 4;
    ASSERT_EQ(expectedHash, fnv1a_hash_data((uint8_t const *)"test", s));
}