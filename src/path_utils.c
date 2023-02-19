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
 * @file path_utils.c
 * @brief Definitions regarding the path utilities used by the emulator
 */


#include "path_utils.h"

// OS-specific definitions
#if defined(OS_WINDOWS)
#define FILE_SEPERATOR ('\\')
#elif defined(OS_UNIX_LIKE)
#define FILE_SEPERATOR ('/')
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

int path_utils_get_executable_path(char * buffer, size_t bufferSize) {
#if defined(OS_WINDOWS)
    return !GetModuleFileName(NULL, buffer, bufferSize);
#elif defined(OS_UNIX_LIKE)
    return !MIN(readlink("/proc/self/exe", buffer, bufferSize), bufferSize - 1);
#endif
}

void path_utils_remove_file_layer(char * buffer, size_t depth) {
    depth--;
    for (size_t i = strlen(buffer); i > 0; i--) {
        if (buffer[i - 1] == FILE_SEPERATOR) {
            if(!depth) {
                buffer[i] = '\0';
                break;
            }
            else {
                depth--;
            }
        }
    }
}

void path_utils_concatenate_folder(char * buffer, char * folderName) {
    strcat(buffer, folderName);
    char fileSeperatorString[2];
    fileSeperatorString[0] = FILE_SEPERATOR;
    fileSeperatorString [1] = '\0';
    strcat(buffer, fileSeperatorString);
}