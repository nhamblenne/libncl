#ifndef LIBNCL_NCL_LEXER_H
#define LIBNCL_NCL_LEXER_H

/* =======================================================================
 * ncl_lexer.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdbool.h>

typedef enum ncl_token_kind {
    ncl_error_tk,
    ncl_reserved_tk,
    ncl_eof_tk,
    ncl_eol_tk,
    ncl_id_tk,
    ncl_operator_tk,
    ncl_number_tk,
    ncl_string_tk,
    ncl_istring_tk,
    ncl_istring_start_tk,
    ncl_istring_cont_tk,
    ncl_istring_end_tk,
    ncl_zstring_tk,
    ncl_semicolon_tk,
    ncl_openpar_tk,
    ncl_closepar_tk,
    ncl_dot_tk,
    ncl_comma_tk,
    ncl_plus_tk,
    ncl_minus_tk,

    ncl_last_tk
} ncl_token_kind;

char const* const ncl_token_names[ncl_last_tk+1];

typedef struct ncl_lexer ncl_lexer;
typedef void (*ncl_lexer_error_func)(ncl_lexer*, char const*);

struct ncl_lexer
{
    ncl_token_kind current_kind;
    char const *current_start;
    char const *current_end;
    char const *buffer_start;
    char const *buffer_pos;
    char const *buffer_end;
    int line_number;
    ncl_lexer_error_func error_func;
};

ncl_token_kind ncl_lex(ncl_lexer*, bool skip_eol);

#endif
