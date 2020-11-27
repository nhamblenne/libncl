/* =======================================================================
 * ncl_value.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_value.h"

#include <assert.h>
#include "ngc.h"
#include "ncl_interp.h"

ncl_value_type ncl_get_type(ncl_value value)
{
    return ((uintptr_t)value) & 0xFU;
}

ncl_value ncl_make_void_value()
{
    return 0;
}

bool ncl_get_bool_value(ncl_value value)
{
    return (((uintptr_t) value) >> 4U) != 0;
}

ncl_value ncl_make_bool(ncl_interp* interp, bool value)
{
    return (ncl_value)(uintptr_t)((value ? 0x10U : 0U) | ncl_bool_type);
}

intptr_t ncl_get_int_value(ncl_value value)
{
    return (intptr_t)value >> 4;
}

ncl_value ncl_make_int(ncl_interp* interp, intptr_t value)
{
    return (ncl_value)((value << 4) | ncl_int_type);
}

char const* ncl_get_symbol_name(ncl_value value)
{
    return (char const*)(((uintptr_t)value) & ~0x0FU);
}

ncl_value ncl_make_symbol(ncl_interp* interp, char const *name)
{
    assert((((uintptr_t) name) & 0xF) == 0);
    return (ncl_value)(((uintptr_t)name) | ncl_symbol_type);
}

char const* ncl_get_string_value(ncl_value value)
{
    return (char const*)(((uintptr_t)value) & ~0x0FU);
}

ncl_value ncl_make_string(ncl_interp* interp, char const *value)
{
    return (ncl_value)(((uintptr_t)value) | ncl_string_type);
}