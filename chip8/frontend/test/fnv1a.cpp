#include <gtest/gtest.h>

#include <stdint.h>

extern "C" {
#include "fnv1a.h"
}

TEST(FNV1A, test) {
    ASSERT_EQ(0xAFD071E5, fnv1a_hash_data((uint8_t const *)"test", 4));
}