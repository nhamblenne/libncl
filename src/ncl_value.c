/* =======================================================================
 * ncl_value.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#define _POSIX_C_SOURCE 200809L

#include "ncl_value.h"

#include <assert.h>
#include <string.h>
#include "ngc.h"
#include "ncl_interp.h"
#include "ncl_interp.pvt.h"

ncl_value_type ncl_get_type(ncl_value value)
{
    return value.repr & 0xFU;
}

ncl_value ncl_make_void_value()
{
    return (ncl_value) {0};
}

bool ncl_get_bool_value(ncl_value value)
{
    return (value.repr >> 4U) != 0;
}

ncl_value ncl_make_bool(ncl_interp *interp, bool value)
{
    (void)interp;
    return (ncl_value){(uintptr_t)((value ? 0x10U : 0U) | ncl_bool_type)};
}

intptr_t ncl_get_int_value(ncl_value value)
{
    return ((intptr_t)value.repr) >> 4;
}

ncl_value ncl_make_int(ncl_interp* interp, intptr_t value)
{
    (void)interp;
    return (ncl_value){(((uintptr_t)value << 4) | ncl_int_type)};
}

char const* ncl_get_symbol_name(ncl_value value)
{
    return (char const*)(value.repr & ~0x0FLLU);
}

ncl_value ncl_make_symbol(ncl_interp* interp, char const *name)
{
    void* result = ncl_hash_get(&interp->symbols, (void*)name);
    if (result == NULL) {
        size_t sz = strlen(name)+1;
        result = malloc(sz + 16);
        uintptr_t mem = (uintptr_t) result;
        mem = (mem + 0xF) & ~(uintptr_t)0xF;
        result = (void*) mem;
        strcpy(result, name);
        ncl_hash_add(&interp->symbols, result);
    }
    assert((((uintptr_t) result) & 0xF) == 0);
    return (ncl_value){((uintptr_t)result) | ncl_symbol_type };
}

char const* ncl_get_string_value(ncl_value value)
{
    return (char const*)(value.repr & ~0x0FLLU);
}

ncl_value ncl_make_string(ncl_interp* interp, char const *value)
{
    (void)interp;
    assert((((uintptr_t) value) & 0xF) == 0);
    return (ncl_value){(((uintptr_t)value) | ncl_string_type)};
}
