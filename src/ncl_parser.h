#ifndef LIBNCL_NCL_PARSER_H
#define LIBNCL_NCL_PARSER_H

#include <stdlib.h>

#include "ncl_lexer.h"

/* =======================================================================
 * ncl_parser.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

typedef struct ncl_node ncl_node;

typedef enum ncl_node_kind {
    ncl_error_node,
    ncl_number_node,
    ncl_id_node,
    ncl_string_node,
    ncl_field_node,
    ncl_call_node,
    ncl_call1_node,
    ncl_unary_node,
    ncl_binary_node,
    ncl_pass_node,
    ncl_exit_node,
    ncl_next_node,
    ncl_return_node,
    ncl_list_node,
    ncl_assign_node,
    ncl_cond_node,
    ncl_while_node,
    ncl_loop_node,
    ncl_for_node,
    ncl_label_node,
    ncl_flambda_node,
    ncl_plambda_node,
} ncl_node_kind;

typedef struct ncl_node_list {
    ncl_node *head;
    ncl_node *tail;
} ncl_node_list;

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

typedef struct ncl_node_unary_exp {
    ncl_token_kind op;
    ncl_node *arg;
} ncl_node_unary_exp;

typedef struct ncl_node_binary_exp {
    ncl_token_kind op;
    ncl_node *left;
    ncl_node *right;
} ncl_node_binary_exp;

typedef struct ncl_node_exp {
    ncl_node *exp;
} ncl_node_exp;

typedef struct ncl_node_assign_stmt {
    ncl_node *to;
    ncl_node *what;
} ncl_node_assign_stmt;

typedef struct ncl_node_cond_stmt {
    ncl_node *cond;
    ncl_node *then_stmt;
    ncl_node *else_stmt;
} ncl_node_cond_stmt;

typedef struct ncl_node_block_stmt {
    ncl_node *block;
} ncl_node_block_stmt;

typedef struct ncl_node_for_stmt {
    ncl_node *var;
    ncl_node *exp;
    ncl_node *block;
} ncl_node_for_stmt;

typedef struct ncl_node_while_stmt {
    ncl_node *cond;
    ncl_node *block;
} ncl_node_while_stmt;

typedef struct ncl_node_lambda_exp {
    ncl_node *args;
    ncl_node *content;
} ncl_node_lambda_exp;

struct ncl_node {
    ncl_node_kind kind;
    union ncl_node_dispatch {
        ncl_node_list list;
        ncl_node_token_exp token;
        ncl_node_field_exp field;
        ncl_node_call_exp call;
        ncl_node_unary_exp unary;
        ncl_node_binary_exp binary;
        ncl_node_exp exp;
        ncl_node_assign_stmt assign;
        ncl_node_cond_stmt cond;
        ncl_node_block_stmt block;
        ncl_node_while_stmt while_stmt;
        ncl_node_for_stmt for_stmt;
        ncl_node_lambda_exp lambda;
    } u;
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

ncl_parse_result ncl_parse(char const *current, size_t size);
void ncl_free_node(ncl_node*);

#endif
