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
 * @file table_code_gen.h
 * @brief Macros used to generate hashtable's for the project
 */

#ifndef CHIP8_CODE_GENERATION_TABLE_H_
#define CHIP8_CODE_GENERATION_TABLE_H_

#include "memory.h"
#include "table.h"

/// Generates a table entry that stores a value of the specified type as data
#define GENERATE_TABLE_ENTRY_TYPE(TYPE) \
    typedef struct {                    \
        char const * key;               \
        TYPE data;                      \
    } TYPE##_table_entry_t;

/// Generates a table that stores the specified type
#define GENERATE_TABLE_TYPE(TYPE)        \
    typedef struct {                     \
        size_t allocated;                \
        size_t used;                     \
        TYPE##_table_entry_t ** entries; \
    } TYPE##_table_t;

/// Generates the function prototypes of a hashtable
#define GENERATE_TABLE_PROTOTYPES(TYPE)                                                                     \
    TYPE##_table_entry_t * TYPE##_table_entry_new(TYPE opcodeAddress, char const * key);                    \
    int TYPE##_table_init_table(TYPE##_table_t * table);                                                    \
    TYPE##_table_t * address_table_new();                                                                   \
    void TYPE##_table_destory(TYPE##_table_t ** table);                                                     \
    void TYPE##_table_free_entries(TYPE##_table_t * table);                                                 \
    int TYPE##_table_insert_entry(TYPE##_table_entry_t * entry, TYPE##_table_t * table);                    \
    TYPE##_table_entry_t * TYPE##_table_remove_entry(TYPE##_table_entry_t * entry, TYPE##_table_t * table); \
    TYPE##_table_entry_t * TYPE##_table_look_up_entry(char const * key, TYPE##_table_t * table);

/// Generates the functions of a hashtable
#define GENERATE_TABLE_FUNCTIONS(TYPE, HASH_FUNCTION)                                                             \
    static int TYPE##_table_grow_table(TYPE##_table_t *);                                                        \
    TYPE##_table_t * TYPE##_table_new() {                                                                        \
        TYPE##_table_t * table = new (TYPE##_table_t);                                                           \
        if (!table) {                                                                                            \
            return NULL;                                                                                         \
        }                                                                                                        \
        TYPE##_table_init_table(table);                                                                          \
        return table;                                                                                            \
    }                                                                                                            \
    TYPE##_table_entry_t * TYPE##_table_entry_new(TYPE address, char const * key) {                              \
        TYPE##_table_entry_t * entry = new (TYPE##_table_entry_t);                                               \
        if (!entry) {                                                                                            \
            return NULL;                                                                                         \
        }                                                                                                        \
        entry->data = address;                                                                                   \
        entry->key = key;                                                                                        \
        return entry;                                                                                            \
    }                                                                                                            \
    void TYPE##_table_destory(TYPE##_table_t ** table) {                                                         \
        if (!*table) {                                                                                           \
            return;                                                                                              \
        }                                                                                                        \
        TYPE##_table_free_entries(*table);                                                                       \
        free(*table);                                                                                            \
        *table = NULL;                                                                                           \
    }                                                                                                            \
    int TYPE##_table_init_table(TYPE##_table_t * table) {                                                        \
        if (!table) {                                                                                            \
            return -1;                                                                                           \
        }                                                                                                        \
        table->used = 0;                                                                                         \
        table->allocated = TABLE_INIT_SIZE;                                                                      \
        table->entries = malloc(table->allocated * sizeof(TYPE##_table_entry_t));                                \
        if (!table->entries) {                                                                                   \
            return -1;                                                                                           \
        }                                                                                                        \
        for (size_t i = 0; i < table->allocated; i++) {                                                          \
            table->entries[i] = NULL;                                                                            \
        }                                                                                                        \
        return 0;                                                                                                \
    }                                                                                                            \
    void TYPE##_table_free_entries(TYPE##_table_t * table) {                                                     \
        if (!table->entries) {                                                                                   \
            return;                                                                                              \
        }                                                                                                        \
        for (size_t i = 0; i < table->allocated; i++) {                                                          \
            if (!table->entries[i] || table->entries[i] == (TYPE##_table_entry_t *)(0xFFFFFFFFFFFFFFFFUL)) {     \
                break;                                                                                           \
            }                                                                                                    \
            if (table->entries[i]->key) {                                                                        \
                free((char *)table->entries[i]->key);                                                            \
            }                                                                                                    \
        }                                                                                                        \
        free(table->entries);                                                                                    \
        table->allocated = table->used = 0;                                                                      \
        table->entries = NULL;                                                                                   \
    }                                                                                                            \
    int TYPE##_table_insert_entry(TYPE##_table_entry_t * node, TYPE##_table_t * table) {                         \
        if (!node || !table) {                                                                                   \
            return -1;                                                                                           \
        }                                                                                                        \
        if (table->used >= ((double)table->allocated) * TABLE_GROWTH_TRIGGER_VALUE) {                            \
            if (TYPE##_table_grow_table(table)) {                                                                \
                return -1;                                                                                       \
            }                                                                                                    \
        }                                                                                                        \
        uint32_t index, try;                                                                                     \
        index = HASH_FUNCTION((uint8_t *)node->key, strlen(node->key));                                           \
        for (size_t i = 0; i < table->allocated; i++) {                                                          \
            try = (i + index) & (table->allocated - 1);                                                          \
            if (!table->entries[try] || table->entries[try] == (TYPE##_table_entry_t *)(0xFFFFFFFFFFFFFFFFUL)) { \
                table->entries[try] = node;                                                                      \
                table->used++;                                                                                   \
                return 0;                                                                                        \
            }                                                                                                    \
        }                                                                                                        \
        return -1;                                                                                               \
    }                                                                                                            \
    TYPE##_table_entry_t * TYPE##_table_remove_entry(TYPE##_table_entry_t * node, TYPE##_table_t * table) {      \
        if (!table) {                                                                                            \
            return NULL;                                                                                         \
        }                                                                                                        \
        uint32_t index = HASH_FUNCTION((uint8_t *)node->key, strlen(node->key));                                  \
        for (uint32_t i = 0; i < table->allocated; i++) {                                                        \
            uint32_t try = (i + index) & (table->allocated - 1);                                                 \
            if (!table->entries[try]) {                                                                          \
                return false;                                                                                    \
            }                                                                                                    \
            if (!strncmp(table->entries[try]->key, node->key, MAX_KEY_LENGTH)) {                                 \
                uint16_t_table_entry_t * tempNode = table->entries[try];                                         \
                table->entries[try] = (uint16_t_table_entry_t *)(0xFFFFFFFFFFFFFFFFUL);                          \
                table->used--;                                                                                   \
                return tempNode;                                                                                 \
            }                                                                                                    \
        }                                                                                                        \
        return NULL;                                                                                             \
    }                                                                                                            \
    TYPE##_table_entry_t * TYPE##_table_look_up_entry(char const * key, TYPE##_table_t * table) {                \
        if (!table) {                                                                                            \
            return NULL;                                                                                         \
        }                                                                                                        \
        uint32_t index = HASH_FUNCTION((uint8_t *)key, strlen(key));                                              \
        for (uint32_t i = 0; i < table->allocated; i++) {                                                        \
            uint32_t try = (i + index) & (table->allocated - 1);                                                 \
            if (!table->entries[try]) {                                                                          \
                return NULL;                                                                                     \
            }                                                                                                    \
            if (!strncmp(table->entries[try]->key, key, MAX_KEY_LENGTH)) {                                       \
                return table->entries[try];                                                                      \
            }                                                                                                    \
        }                                                                                                        \
        return NULL;                                                                                             \
    }                                                                                                            \
    static int TYPE##_table_grow_table(TYPE##_table_t * table) {                                                 \
        TYPE##_table_entry_t ** newEntries =                                                                     \
            CHECKED_MALLOC_USING_TYPE(table->allocated * GROWTH_FACTOR, *table->entries);                        \
        if (!newEntries) {                                                                                       \
            return -1;                                                                                           \
        }                                                                                                        \
        for (size_t i = 0; i < table->allocated * GROWTH_FACTOR; i++) {                                          \
            newEntries[i] = NULL;                                                                                \
        }                                                                                                        \
        TYPE##_table_entry_t ** oldEntries = table->entries;                                                     \
        table->entries = newEntries;                                                                             \
        table->used = 0;                                                                                         \
        table->allocated *= GROWTH_FACTOR;                                                                       \
        for (size_t j = 0; j < table->allocated / GROWTH_FACTOR; j++) {                                          \
            TYPE##_table_insert_entry(oldEntries[j], table);                                                     \
        }                                                                                                        \
        free(oldEntries);                                                                                        \
        return 0;                                                                                                \
    }

#endif
