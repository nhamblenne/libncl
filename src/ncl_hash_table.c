/* =======================================================================
 * ncl_hash_table.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_hash_table.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define DELETED      ((void*)(uintptr_t)-1)
#define NEXT(i, max) (((i)+1 == (max)) ? 0 : (i)+1)
#define PREV(i, max) (((i) == 0) ? (max) - 1 : (i) - 1)

// If I wasn't NIH, I would not try to do in place cleaning for hash tables without
// measures showing it has a clear benefit; but I'm NIH and what I do is for my fun,
// reinventing the wheel and over-engineering is part of the game.
static void hash_clean_table(ncl_hash_table *table)
{
    // find a NULL element in the table, it will serve as stop point
    size_t boundary = 0;
    while (table->content[boundary] != NULL) {
        ++boundary;
        assert(boundary < table->allocated);
    }
    // top is the element to be considered, the only thing we know about top
    // is that it is not part of the same chain as top+1 (top+1 being NULL is
    // the trivial starting case)
    size_t top = boundary == 0 ? table->allocated-1 : boundary-1;
    // we can move the boundary up as long as it is NULL
    while (boundary+1 < table->allocated && table->content[boundary+1] == NULL) {
        ++boundary;
    }
    while (top != boundary) {
        // DELETED elements followed by NULL can be replaced by NULL
        while (table->content[top] == DELETED) {
            assert(top != boundary);
            table->content[top] = NULL;
            --table->deleted;
            top = PREV(top, table->allocated);
        }
        if (table->content[top] != NULL) {
            assert(table->content[top] != DELETED);
            // we are at the top of a run of non NULL elements, try to move some of them down
            // in place of the the DELETED elements it contains
            size_t cur = top;
            bool element_moved = false;
            while (table->content[cur] != NULL && table->content[cur] != DELETED) {
                void const *elem = table->content[cur];
                table->content[cur] = DELETED;
                ++table->deleted;
                --table->used;
                ncl_hash_add(table, elem);
                element_moved = element_moved || table->content[cur] == DELETED;
                cur = PREV(cur, table->allocated);
            }
            // if we have moved some elements, we have to start examining at top again
            // if not we can move top the current position as cur is not part of the
            // same chain as cur+1
            if (!element_moved) {
                top = cur;
            }
        }
        // find a non NULL element, but do not pass boundary
        while (top != boundary && table->content[top] == NULL) {
            top = PREV(top, table->allocated);
        }
    }
}

static void hash_reallocate(ncl_hash_table *table, size_t needed)
{
    if (needed == 0) {
        free(table->content);
        table->content = NULL;
        table->allocated = 0;
        table->used = 0;
        table->deleted = 0;
    } else if (needed * 5 < table->allocated || table->allocated * 2 < needed * 3) {
        // reallocate it too few or too many elements in the table (less than 20% capacity
        // or more than 66%)
        size_t new_size = ((needed * 4 < table->allocated) || (table->allocated == 0)) ? 16 : table->allocated;
        while (new_size < needed) {
            new_size += new_size/2;
        }
        new_size += new_size;
        void const** old_content = table->content;
        size_t old_allocated = table->allocated;
        table->content = malloc(new_size * sizeof (void*));
        memset(table->content, 0, new_size * sizeof (void*));
        if (table->content == NULL) {
            fprintf(stderr, "Out of memory, aborting.\n");
            abort();
        }
        table->allocated = new_size;
        table->used = 0;
        table->deleted = 0;
        for (size_t i = 0; i < old_allocated; ++i) {
            if (old_content[i] != NULL && old_content[i] != DELETED) {
                ncl_hash_add(table, old_content[i]);
            }
        }
        free(old_content);
    } else {
        hash_clean_table(table);
    }
}

void ncl_hash_init(ncl_hash_table *table)
{
    table->content = NULL;
    table->allocated = 0;
    table->used = 0;
    table->deleted = 0;
    table->equal = NULL;
    table->hash = NULL;
}

bool ncl_hash_add(ncl_hash_table *table, const void *elem)
{
    assert(elem != NULL && elem != DELETED && table != NULL && table->hash != NULL && table->equal != NULL);
    uintptr_t h = table->hash(elem);
    if (table->allocated != 0) {
        uintptr_t cur = h % table->allocated;
        while (table->content[cur] != NULL) {
            if (table->content[cur] != DELETED && table->equal(elem, table->content[cur])) {
                return false;
            }
            cur = NEXT(cur, table->allocated);
        }
    }
    // if we are over 75% capacity when counting the deleted elements, clean up
    if (table->allocated * 3 <= (table->used + table->deleted) * 4) {
        hash_reallocate(table, table->used+1);
    }
    uintptr_t cur = h % table->allocated;
    while (table->content[cur] != NULL && table->content[cur] != DELETED) {
        cur = NEXT(cur, table->allocated);
    }
    if (table->content[cur] == DELETED) {
        --table->deleted;
    }
    ++table->used;
    table->content[cur] = elem;
    return true;
}

void const *ncl_hash_get(ncl_hash_table *table, void const *elem)
{
    assert(elem != NULL && elem != DELETED && table != NULL && table->hash != NULL && table->equal != NULL);
    uintptr_t h = table->hash(elem);
    if (table->allocated != 0) {
        uintptr_t cur = h % table->allocated;
        while (table->content[cur] != NULL) {
            if (table->content[cur] != DELETED && table->equal(elem, table->content[cur])) {
                return table->content[cur];
            }
            cur = NEXT(cur, table->allocated);
        }
    }
    return NULL;
}

void ncl_hash_remove(ncl_hash_table *table, void const *elem)
{
    assert(elem != NULL && elem != DELETED && table != NULL && table->hash != NULL && table->equal != NULL);
    uintptr_t h = table->hash(elem);
    if (table->allocated != 0) {
        uintptr_t cur = h % table->allocated;
        while (table->content[cur] != NULL) {
            if (table->content[cur] != DELETED && table->equal(elem, table->content[cur])) {
                --table->used;
                if (table->content[NEXT(cur, table->allocated)] == NULL) {
                    table->content[cur] = NULL;
                } else {
                    table->content[cur] = DELETED;
                    ++table->deleted;
                }
                break;
            }
            if (++cur == table->allocated) {
                cur = 0;
            }
        }
        if (table->used * 5 < table->allocated && table->allocated > 32) {
            hash_reallocate(table, table->used);
        }
    }
}

void ncl_hash_free(ncl_hash_table *table)
{
    hash_reallocate(table, 0);
}
