#ifndef LIBNCL_NCL_INTERP_PVT_H
#define LIBNCL_NCL_INTERP_PVT_H

/* =======================================================================
 * ncl_interp.pvt.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_hash_table.h"

struct ncl_interp
{
    ncl_hash_table symbols;
};

#endif
