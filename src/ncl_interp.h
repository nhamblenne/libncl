#ifndef LIBNCL_NCL_INTERP_H
#define LIBNCL_NCL_INTERP_H

/* =======================================================================
 * ncl_interp.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

typedef struct ncl_interp ncl_interp;

ncl_interp* ncl_init_interp();
void ncl_free_interp(ncl_interp*);

#endif
