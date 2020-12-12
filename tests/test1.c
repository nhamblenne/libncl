/* =======================================================================
 * test1.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#undef NDEBUG
#include <assert.h>
#include <memory.h>

#include "ncl_interp.h"
#include "ncl_value.h"

int main()
{
    ncl_interp *interp = ncl_init_interp();

    ncl_value vv = ncl_make_void_value();
    assert(ncl_get_type(vv) == ncl_void_type);

    ncl_value bv = ncl_make_bool(interp, false);
    assert(ncl_get_type(bv) == ncl_bool_type);
    assert(ncl_get_bool_value(bv) == false);

    bv = ncl_make_bool(interp, true);
    assert(ncl_get_type(bv) == ncl_bool_type);
    assert(ncl_get_bool_value(bv) == true);

    ncl_value iv = ncl_make_int(interp, 0);
    assert(ncl_get_type(iv) == ncl_int_type);
    assert(ncl_get_int_value(iv) == 0);
    iv = ncl_make_int(interp, -1);
    assert(ncl_get_type(iv) == ncl_int_type);
    assert(ncl_get_int_value(iv) == -1);
    iv = ncl_make_int(interp, 1);
    assert(ncl_get_type(iv) == ncl_int_type);
    assert(ncl_get_int_value(iv) == 1);
    iv = ncl_make_int(interp, INTPTR_MAX/16);
    assert(ncl_get_type(iv) == ncl_int_type);
    assert(ncl_get_int_value(iv) == INTPTR_MAX/16);
    iv = ncl_make_int(interp, INTPTR_MIN/16);
    assert(ncl_get_type(iv) == ncl_int_type);
    assert(ncl_get_int_value(iv) == INTPTR_MIN/16);

    iv = ncl_make_int(interp, INTPTR_MAX);
    assert(ncl_get_type(iv) == ncl_int_type);
    iv = ncl_make_int(interp, INTPTR_MIN);
    assert(ncl_get_type(iv) == ncl_int_type);

    ncl_value sym1 = ncl_make_symbol(interp, "foo");
    assert(ncl_get_type(sym1) == ncl_symbol_type);
    assert(strcmp(ncl_get_symbol_name(sym1), "foo") == 0);
    char const sym2name[] = "foo";
    ncl_value sym2 = ncl_make_symbol(interp, sym2name);
    assert(ncl_get_type(sym2) == ncl_symbol_type);
    assert(strcmp(ncl_get_symbol_name(sym2), "foo") == 0);
    assert(sym1.repr == sym2.repr);

    ncl_free_interp(interp);
}
