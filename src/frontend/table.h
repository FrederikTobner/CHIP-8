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
 * @file table.h
 * @brief Macros used by the hashtable's of the project
 */

#ifndef CHIP8_TABLE_H_
#define CHIP8_TABLE_H_

/// The maximum length of the key of a hashtable entry
#define MAX_KEY_LENGTH                     (1024)

/// The growth factor of a hashtable
#define GROWTH_FACTOR                      (2)

/// The growth trigger value of a hashtables
#define TABLE_GROWTH_TRIGGER_VALUE         (0.75)

/// The initial size of a hashtables
#define TABLE_INIT_SIZE                    (8)

/// Size check using a type (used for the checked malloc)
#define SIZE_CHECK_USING_TYPE(n, type)     ((SIZE_MAX / sizeof(type)) >= (n))

/// @brief Checked malloc
/// @details Treats a overflow parameters when malloc was called like a usual allocation error
#define CHECKED_MALLOC_USING_TYPE(n, type) (SIZE_CHECK_USING_TYPE((n), (type)) ? malloc((n) * sizeof(type)) : 0)

#define new(type) \
 (malloc(sizeof(type)))

#endif