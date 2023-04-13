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
 * @file dynamic_array_code_gen.h
 * @brief Macros used for generating dynamic array's
 */

#ifndef CHIP8_DYNAMIC_ARRAY_CODE_GENERATION_H
#define CHIP8_DYNAMIC_ARRAY_CODE_GENERATION_H

#include "memory.h"

#define MIN(a, b)                         (((a) < (b)) ? (a) : (b))

#define MAX(a, b)                         (((a) > (b)) ? (a) : (b))

#define MIN_ARRAY_SIZE                    (1)

#define MAX_ARRAY_LENGTH_USING_TYPE(type) (SIZE_MAX / sizeof(type))

#define AT_MAX_LENGTH_USING_TYPE(n, type) ((n) == MAX_ARRAY_LENGTH_USING_TYPE(type))

#define CAPPED_DPL_SIZE_USING_TYPE(n, type) \
    (((n) < MAX_ARRAY_LENGTH_USING_TYPE(type) / 2) ? (2 * (n)) : MAX_ARRAY_LENGTH_USING_TYPE(type))

#define MAX_ARRAY_LENGTH_USING_SIZE(obj_size) (SIZE_MAX / obj_size)

#define AT_MAX_LENGTH_USING_SIZE(n, obj_size) ((n) == MAX_ARRAY_LENGTH_USING_SIZE(obj_size))

#define CAPPED_DPL_USING_SIZE(n, obj_size) \
    (((n) < MAX_ARRAY_LENGTH_USING_SIZE(obj_size) / 2) ? (2 * (n)) : MAX_ARRAY_LENGTH_USING_SIZE(obj_size))

#define GENERATE_DYNAMIC_ARRAY_TYPE(TYPE)                               \
    static_assert(sizeof(#TYPE) > 1, "The type needs to be specified"); \
    typedef struct {                                                    \
        size_t allocated;                                               \
        size_t used;                                                    \
        TYPE * data;                                                    \
    } TYPE##_dynamic_array_t;

#define GENERATE_DYNAMIC_ARRAY_PROTOTYPES(TYPE)                                                         \
    static_assert(sizeof(#TYPE) > 1, "The type needs to be specified");                                 \
    bool TYPE##_dynamic_array_init(TYPE##_dynamic_array_t * array, size_t init_size, size_t init_used); \
    void TYPE##_dynamic_array_free(TYPE##_dynamic_array_t * array);                                     \
    bool TYPE##_dynamic_array_resize(TYPE##_dynamic_array_t * array, size_t new_size);                  \
    bool TYPE##_dynamic_array_write(TYPE##_dynamic_array_t * array, TYPE val);

#define GENERATE_DYNAMIC_ARRAY_FUNCTIONS(TYPE)                                                           \
    static_assert(sizeof(#TYPE) > 1, "The type needs to be specified");                                  \
    bool TYPE##_dynamic_array_init(TYPE##_dynamic_array_t * array, size_t init_size, size_t init_used) { \
        assert(init_size >= init_used);                                                                  \
        init_size = MAX(init_size, MIN_ARRAY_SIZE);                                                      \
        array->data = CHECKED_MALLOC_USING_TYPE(init_size, *array->data);                                \
        array->allocated = (array->data) ? init_size : 0;                                                \
        array->used = (array->data) ? init_used : 0;                                                     \
        return !!array->data;                                                                            \
    }                                                                                                    \
                                                                                                         \
    void TYPE##_dynamic_array_free(TYPE##_dynamic_array_t * array) {                                     \
        free(array->data);                                                                               \
        array->data = NULL;                                                                              \
        array->allocated = array->used = 0;                                                              \
    }                                                                                                    \
                                                                                                         \
    bool TYPE##_dynamic_array_resize(TYPE##_dynamic_array_t * array, size_t new_size) {                  \
        size_t alloc_size = MAX(new_size, MIN_ARRAY_SIZE);                                               \
        TYPE * new_data = CHECKED_REALLOC_USING_SIZE(array->data, alloc_size, *array->data);             \
        if (!new_data)                                                                                   \
            return false;                                                                                \
        array->data = new_data;                                                                          \
        array->allocated = alloc_size;                                                                   \
        array->used = MIN(array->used, new_size);                                                        \
        return true;                                                                                     \
    }                                                                                                    \
                                                                                                         \
    bool TYPE##_dynamic_array_write(TYPE##_dynamic_array_t * array, TYPE val) {                          \
        if (array->used == array->allocated) {                                                           \
            if (AT_MAX_LENGTH_USING_TYPE(array->allocated, *array->data))                                \
                return false;                                                                            \
            size_t new_size = CAPPED_DPL_USING_SIZE(array->allocated, *array->data);                     \
            int resize_success = TYPE##_dynamic_array_resize(array, new_size);                           \
            if (!resize_success)                                                                         \
                return false;                                                                            \
        }                                                                                                \
        array->data[array->used++] = val;                                                                \
        return true;                                                                                     \
    }

#endif