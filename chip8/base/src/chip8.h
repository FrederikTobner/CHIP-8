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
 * @file chip8.h
 * @brief Defenitions regarding the vm that need to be accessible by several libraries
 */

#ifndef CHIP8_CHIP8_H_
#define CHIP8_CHIP8_H_

/// Most Chip-8 programs start at location 0x200 (512), but some begin at 0x600 (1536). Programs beginning at 0x600 are
/// intended for the ETI 660 computer. Our emulator will only support programs starting at 0x200 for now.
#define PROGRAM_START_LOCATION (0x200u)

#endif