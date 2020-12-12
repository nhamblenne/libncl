/* =======================================================================
 * ncl_interp.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_interp.h"
#include "ncl_interp.pvt.h"

#include <stdlib.h>
#include <string.h>

static uintptr_t symbol_hash(void const* elt)
{
    uintptr_t const factor = -59LLU; // largest prime less than 2^64
    uintptr_t const offset = -83LLU; // another big prime less than 2^64
    uintptr_t result = 0;
    for (unsigned char const *str = elt; *str != '\0'; ++str) {
        result = (result + *str) * factor ^ offset;
    }
    return result;
}

static bool symbol_equal(void const* e1, void const* e2)
{
    return strcmp(e1, e2) == 0;
}

ncl_interp *ncl_init_interp()
{
    ncl_interp *result = malloc(sizeof(ncl_interp));
    ncl_hash_init(&result->symbols);
    result->symbols.hash = symbol_hash;
    result->symbols.equal = symbol_equal;
    return result;
}

void ncl_free_interp(ncl_interp *interp)
{
    for (unsigned i = 0; i < interp->symbols.allocated; ++i) {
        if (interp->symbols.content[i] != NULL && interp->symbols.content[i] != NCL_DELETED_ENTRY) {
            free(interp->symbols.content[i]);
        }
    }
    ncl_hash_free(&interp->symbols);
    free(interp);
}
