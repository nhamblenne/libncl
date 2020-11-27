/* =======================================================================
 * ncl_interp.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_interp.h"

#include <stdlib.h>

struct ncl_interp
{
};

ncl_interp *ncl_init_interp()
{
    return malloc(sizeof(ncl_interp));
}

void ncl_free_interp(ncl_interp *interp)
{
    free(interp);
}