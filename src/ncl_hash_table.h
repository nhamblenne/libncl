#ifndef LIBNCL_NCL_HASH_TABLE_H
#define LIBNCL_NCL_HASH_TABLE_H

/* =======================================================================
 * ncl_hash_table.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct ncl_hash_table {
    void const** content;
    size_t allocated;
    size_t used;
    size_t deleted;
    bool (*equal)(void const*, void const*);
    uintptr_t (*hash)(void const*);
} ncl_hash_table;

void ncl_hash_init(ncl_hash_table*);
bool ncl_hash_add(ncl_hash_table*, void const*);
void const* ncl_hash_get(ncl_hash_table*, void const*);
void ncl_hash_remove(ncl_hash_table*, void const*);
void ncl_hash_free(ncl_hash_table*);

#endif
