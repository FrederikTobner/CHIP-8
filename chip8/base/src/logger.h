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
 * @file logger.h
 * @brief The logger of the emulator
 */

#ifndef CHIP8_LOGGER_H
#define CHIP8_LOGGER_H

#ifdef BUILD_TYPE_DEBUG
#include <stdarg.h>
#include <stdio.h>

#define C_RESET           "\x1B[0m"  // White (Default)
#define C_DEBUG           "\x1B[32m" // Green
#define C_INFO            "\x1B[36m" // Light-Blue
#define C_WARN            "\x1B[33m" // Yellow
#define C_ERROR           "\x1B[31m" // Red
#define log_debug(M, ...) fprintf(stderr, "[%sDEBUG%s]\t - " M "\n", C_DEBUG, C_RESET, ##__VA_ARGS__)
#define log_info(M, ...)  fprintf(stderr, "[%sINFO%s]\t - " M "\n", C_INFO, C_RESET, ##__VA_ARGS__)
#define log_warn(M, ...)  fprintf(stderr, "[%sWARN%s]\t - " M "\n", C_WARN, C_RESET, ##__VA_ARGS__)
#define log_err(M, ...)   fprintf(stderr, "[%sERROR%s]\t - " M "\n", C_ERROR, C_RESET, ##__VA_ARGS__)

#define log_debug_at(M, ...) \
    fprintf(stderr, "[%sDEBUG%s]\t%s(%d)\ - " M "\n", C_DEBUG, C_RESET, __FILE__, __LINE__, ##__VA_ARGS__)
#define log_info_at(M, ...) \
    fprintf(stderr, "[%sINFO%s]\t%s(%d)\t - " M "\n", C_INFO, C_RESET, __FILE__, __LINE__, ##__VA_ARGS__)
#define log_err_at(M, ...) \
    fprintf(stderr, "[%sERROR%s]\t%s(%d)\t - " M "\n", C_ERROR, C_RESET, __FILE__, __LINE__, ##__VA_ARGS__)
#define log_warn_at(M, ...) \
    fprintf(stderr, "[%sWARN%s]\t%s(%d)\t - " M "\n", C_WARN, C_RESET, __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define log_debug(M, ...)
#define log_info(M, ...)
#define log_warn(M, ...)
#define log_err(M, ...)

#define log_debug_at(M, ...)
#define log_info_at(M, ...)
#define log_err_at(M, ...)
#define log_warn_at(M, ...)
#endif

#endif