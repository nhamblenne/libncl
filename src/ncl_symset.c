/* =======================================================================
 * ncl_symset.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_symset.h"

ncl_symset ncl_init_symset()
{
    ncl_symset result;
    for (int i = 0; i < NCL_SYMSET_SIZE; ++i) {
        result.elem[0] = 0;
    }
    return result;
}

ncl_symset ncl_symset_singleton(ncl_token_kind tk)
{
    ncl_symset result;
    for (int i = 0; i < NCL_SYMSET_SIZE; ++i) {
        result.elem[0] = 0;
    }
    result.elem[tk / NCL_UINTMAX_BIT_SIZE] = 1 << (tk % NCL_UINTMAX_BIT_SIZE);
    return result;
}

ncl_symset ncl_symset_add_elem(ncl_symset set, ncl_token_kind tk)
{
    ncl_symset result = set;
    result.elem[tk / NCL_UINTMAX_BIT_SIZE] |= 1 << (tk % NCL_UINTMAX_BIT_SIZE);
    return result;
}

ncl_symset ncl_symset_remove_elem(ncl_symset set, ncl_token_kind tk)
{
    ncl_symset result = set;
    result.elem[tk / NCL_UINTMAX_BIT_SIZE] &= ~(uintmax_t)1 << (tk % NCL_UINTMAX_BIT_SIZE);
    return result;
}

ncl_symset ncl_symset_or(ncl_symset a, ncl_symset b)
{
    ncl_symset result;
    for (int i = 0; i < NCL_SYMSET_SIZE; ++i) {
        result.elem[i] = a.elem[i] | b.elem[i];
    }
    return result;
}

ncl_symset ncl_symset_and(ncl_symset a, ncl_symset b)
{
    ncl_symset result;
    for (int i = 0; i < NCL_SYMSET_SIZE; ++i) {
        result.elem[i] = a.elem[i] & b.elem[i];
    }
    return result;
}

bool ncl_symset_is_empty(ncl_symset set)
{
    for (int i = 1; i < NCL_SYMSET_SIZE; ++i) {
        if (set.elem[i] != 0) {
            return false;
        }
    }
    return set.elem[0] == 0;
}

bool ncl_symset_has_elem(ncl_symset set, ncl_token_kind tk)
{
    return (set.elem[tk / NCL_UINTMAX_BIT_SIZE] & (1 << (tk % NCL_UINTMAX_BIT_SIZE))) != 0;
}
