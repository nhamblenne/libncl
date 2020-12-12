#ifndef LIBNCL_NODE_DUMPER_H
#define LIBNCL_NODE_DUMPER_H

/* =======================================================================
 * node-dumper.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

typedef struct ncl_node ncl_node;
void show_node(int indent, ncl_node *top);

#endif
