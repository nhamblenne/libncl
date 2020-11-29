#ifndef LIBNCL_NCL_PARSER_H
#define LIBNCL_NCL_PARSER_H

/* =======================================================================
 * ncl_parser.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

typedef struct ncl_node *ncl_node;
typedef enum ncl_parse_result {
    ncl_parse_error,
    ncl_parse_incomplete,
    ncl_parse_ok
} ncl_parse_result;

ncl_parse_result ncl_parse(char const*);

#endif
