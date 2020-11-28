/* =======================================================================
 * test2.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "ncl_hash_table.h"

uintptr_t hash(void const* elt)
{
    uintptr_t const factor = -59LLU; // largest prime less than 2^64
    uintptr_t const offset = -83LLU; // another big prime less than 2^64
    unsigned char const* str = elt;
    uintptr_t result = 0;
    for (unsigned char const *str = elt; *str != '\0'; ++str) {
        result = (result + *str) * factor ^ offset;
    }
    return result;
}

bool equal(void const* e1, void const* e2)
{
    return strcmp(e1, e2) == 0;
}

int main()
{
    ncl_hash_table table;
    ncl_hash_init(&table);
    table.hash = hash;
    table.equal = equal;
    char *k1 = "foo";
    char *k2 = "true";
    ncl_hash_add(&table, k1);
    ncl_hash_add(&table, k2);
    ncl_hash_add(&table, "false");
    char key[] = "foo";
    assert(k1 == ncl_hash_get(&table, key));
    assert(k2 == ncl_hash_get(&table, "true"));
    ncl_hash_remove(&table, "true");
    assert(ncl_hash_get(&table, "true") == NULL);
    for (int i = 0; i < 200; ++i) {
        char key[20];
        sprintf(key, "%d", i);
        ncl_hash_add(&table, strdup(key));
    }
    for (int i = 0; i < 200; ++i) {
        char key[20];
        sprintf(key, "%d", i);
        assert(ncl_hash_get(&table, key) != NULL);
    }
    for (int i = 0; i < 120; ++i) {
        char key[20];
        sprintf(key, "%d", i);
        ncl_hash_remove(&table, key);
        for (int j = i+1; j < 200; ++j) {
            char key2[20];
            sprintf(key2, "%d", j);
            assert(ncl_hash_get(&table, key2) != NULL);
        }
    }
    for (int i = 0; i < 120; ++i) {
        char key[20];
        sprintf(key, "%d", i);
        assert(ncl_hash_get(&table, key) == NULL);
    }
    for (int i = 120; i < 200; ++i) {
        char key[20];
        sprintf(key, "%d", i);
        assert(ncl_hash_get(&table, key) != NULL);
        ncl_hash_remove(&table, key);
        assert(ncl_hash_get(&table, key) == NULL);
        for (int j = i+1; j < 200; ++j) {
            char key2[20];
            sprintf(key2, "%d", j);
            assert(ncl_hash_get(&table, key2) != NULL);
        }
    }
    ncl_hash_free(&table);
}