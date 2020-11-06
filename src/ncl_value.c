/* =======================================================================
 * ncl_value.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_value.h"
#include "ngc.h"

ncl_interp ncl_make_interp()
{
    ngc_collect(true);
}
