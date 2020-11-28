#ifndef LIBNCL_NCL_VALUE_H
#define LIBNCL_NCL_VALUE_H

/* =======================================================================
 * ncl_value.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */
#include <stdbool.h>
#include <stdint.h>

typedef struct ncl_value {
    uintptr_t repr;
} ncl_value;

typedef struct ncl_interp ncl_interp;

typedef enum ncl_value_type {
    ncl_void_type,
    ncl_bool_type,
    ncl_int_type,
    ncl_symbol_type,
    ncl_string_type
} ncl_value_type;

ncl_value_type ncl_get_type(ncl_value);

ncl_value ncl_make_void_value();

bool ncl_get_bool_value(ncl_value);
ncl_value ncl_make_bool(ncl_interp*, bool);

intptr_t ncl_get_int_value(ncl_value);
ncl_value ncl_make_int(ncl_interp*, intptr_t);

char const* ncl_get_symbol_name(ncl_value);
ncl_value ncl_make_symbol(ncl_interp*, char const*);

char const* ncl_get_string_value(ncl_value);
ncl_value ncl_make_string(ncl_interp*, char const*);

#endif
