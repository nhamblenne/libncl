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
    ncl_punctuation_tk,
    ncl_integer_tk,
    ncl_string_tk,
    ncl_istring_tk,
    ncl_istring_start_tk,
    ncl_istring_cont_tk,
    ncl_istring_end_tk
} ncl_token_kind;

typedef struct ncl_lexer_result
{
    ncl_token_kind kind;
    char const *start;
    char const *end;
} ncl_lexer_result;

ncl_lexer_result ncl_lex(char const* cur, char const* end, bool skipEOL);

#endif
