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
#endif

int path_utils_get_executable_path(char * buffer, size_t bufferSize) {
#if defined(OS_WINDOWS)
    return !GetModuleFileName(NULL, buffer, bufferSize);
#elif defined(OS_UNIX_LIKE)
    return !readlink("/proc/self/exe", buffer, bufferSize);
#endif
}

void path_utils_remove_file_layer(char * buffer, size_t depth) {
    if (!depth) {
        return;
    }
    size_t bufferLength = strlen(buffer);
    // Removing trailing file seperator
    if (buffer[bufferLength - 1] == FILE_SEPERATOR) {
        buffer[--bufferLength] = '\0';
    }
    for (size_t i = bufferLength - 1; i > 0; i--) {
        if (buffer[i] == FILE_SEPERATOR) {
            if (depth == 1) {
                // We don't remove the file seperator at the end
                buffer[i + 1] = '\0';
                break;
            } else {
                depth--;
            }
        }
    }
}

void path_utils_concatenate_folder(char * buffer, char * folderName, size_t maxBufferSize) {
    if (strlen(buffer) + strlen(folderName) + 2 > maxBufferSize) {
        return;
    }
    strcat(buffer, folderName);
    char fileSeperatorString[2];
    fileSeperatorString[0] = FILE_SEPERATOR;
    fileSeperatorString[1] = '\0';
    strcat(buffer, fileSeperatorString);
}