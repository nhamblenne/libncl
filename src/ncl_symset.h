#ifndef LIBNCL_NCL_SYMSET_H
#define LIBNCL_NCL_SYMSET_H

/* =======================================================================
 * ncl_symset.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdint.h>
#include <limits.h>

#include "ncl_lexer.h"

#define NCL_UINTMAX_BIT_SIZE (sizeof(uintmax_t)*CHAR_BIT)
#define NCL_SYMSET_SIZE ((ncl_last_tk + NCL_UINTMAX_BIT_SIZE - 1)/NCL_UINTMAX_BIT_SIZE)

typedef struct ncl_symset {
    uintmax_t elem[NCL_SYMSET_SIZE];
} ncl_symset;

ncl_symset ncl_init_symset();
ncl_symset ncl_symset_singleton(ncl_token_kind);
ncl_symset ncl_symset_add_elem(ncl_symset, ncl_token_kind);
ncl_symset ncl_symset_remove_elem(ncl_symset, ncl_token_kind);
ncl_symset ncl_symset_or(ncl_symset, ncl_symset);
ncl_symset ncl_symset_and(ncl_symset, ncl_symset);

bool ncl_symset_is_empty(ncl_symset);
bool ncl_symset_has_elem(ncl_symset, ncl_token_kind);

#endif
