#ifndef LIBNCL_NCL_PARSER_H
#define LIBNCL_NCL_PARSER_H

/* =======================================================================
 * ncl_parser.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

typedef struct ncl_node ncl_node;

typedef enum ncl_node_kind {
    ncl_error_node,
    ncl_statements_node,
    ncl_number_node,
    ncl_id_node,
    ncl_string_node,
    ncl_field_node,
    ncl_call_node,
    ncl_args_node,
} ncl_node_kind;

typedef struct ncl_node_statements {
    ncl_node *head;
    ncl_node *tail;
} ncl_node_statements;

typedef struct ncl_node_token_exp {
    char const *start;
    char const *end;
} ncl_node_token_exp;

typedef struct ncl_node_field_exp {
    ncl_node *exp;
    char const *start;
    char const *end;
} ncl_node_field_exp;

typedef struct ncl_node_call_exp {
    ncl_node *func;
    ncl_node *args;
} ncl_node_call_exp;

struct ncl_node {
    ncl_node_kind kind;
    union {
        ncl_node_statements list;
        ncl_node_token_exp token;
        ncl_node_field_exp field;
        ncl_node_call_exp call;
    };
};

typedef enum ncl_parse_result_enum {
    ncl_parse_ok,
    ncl_parse_incomplete,
    ncl_parse_error,
    ncl_parse_none,
} ncl_parse_result_enum;

typedef struct ncl_parse_result {
    ncl_parse_result_enum error;
    ncl_node *top;
} ncl_parse_result;

ncl_parse_result ncl_parse(char const*);
void ncl_free_node(ncl_node*);

#endif
