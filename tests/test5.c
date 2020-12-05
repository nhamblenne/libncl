/* =======================================================================
 * test5.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <assert.h>

#include "ncl_symset.h"

int main()
{
    ncl_symset a = ncl_init_symset();
    assert(ncl_symset_is_empty(a));
    ncl_symset b = a;
    a = ncl_symset_add_elem(a, ncl_eof_tk);
    assert(!ncl_symset_is_empty(a));
    assert(ncl_symset_has_elem(a, ncl_eof_tk));
    a = ncl_symset_add_elem(a, ncl_semicolon_tk);
    assert(ncl_symset_has_elem(a, ncl_semicolon_tk));
    ncl_symset c = a;
    assert(!ncl_symset_is_empty(c));
    assert(ncl_symset_has_elem(c, ncl_eof_tk));
    assert(ncl_symset_has_elem(c, ncl_semicolon_tk));
    b = ncl_symset_add_elem(b, ncl_id_tk);
    assert(ncl_symset_has_elem(b, ncl_id_tk));
    b = ncl_symset_add_elem(b, ncl_eof_tk);
    assert(ncl_symset_has_elem(b, ncl_eof_tk));
    c = ncl_symset_or(a, b);
    assert(ncl_symset_has_elem(c, ncl_eof_tk));
    assert(ncl_symset_has_elem(c, ncl_semicolon_tk));
    assert(ncl_symset_has_elem(c, ncl_id_tk));
    c = ncl_symset_and(a, b);
    assert(ncl_symset_has_elem(c, ncl_eof_tk));
    assert(!ncl_symset_has_elem(c, ncl_semicolon_tk));
    assert(!ncl_symset_has_elem(c, ncl_id_tk));
    return 0;
}