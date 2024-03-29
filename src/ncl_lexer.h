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
    ncl_mult_tk,
    ncl_div_tk,
    ncl_ampersand_tk,
    ncl_eq_tk,
    ncl_ne_tk,
    ncl_lt_tk,
    ncl_le_tk,
    ncl_ge_tk,
    ncl_gt_tk,
    ncl_assign_tk,
    ncl_colon_tk,
    ncl_return_tk,
    ncl_openbraces_tk,
    ncl_closebraces_tk,

    ncl_idiv_kw,
    ncl_mod_kw,
    ncl_rem_kw,
    ncl_not_kw,
    ncl_and_kw,
    ncl_or_kw,
    ncl_pass_kw,
    ncl_exit_kw,
    ncl_next_kw,
    ncl_return_kw,
    ncl_when_kw,
    ncl_unless_kw,
    ncl_if_kw,
    ncl_then_kw,
    ncl_elsif_kw,
    ncl_else_kw,
    ncl_end_kw,
    ncl_begin_kw,
    ncl_while_kw,
    ncl_loop_kw,
    ncl_for_kw,
    ncl_in_kw,
    ncl_do_kw,

    ncl_last_tk
} ncl_token_kind;

extern char const* const ncl_token_names[ncl_last_tk+1];

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
