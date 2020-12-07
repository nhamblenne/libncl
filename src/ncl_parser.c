/* =======================================================================
² * ncl_parser.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "ncl_lexer.h"
#include "ncl_symset.h"

static bool dynamic_initialization_done = false;
static ncl_symset basic_expression_starter_set;
static ncl_symset basic_expression_closepar_set;
static ncl_symset postfix_expression_incall_set;
static ncl_symset postfix_expression_starter_set;
static ncl_symset postfix_expression_next_set;
static ncl_symset unary_call_expression_starter_set;
static ncl_symset unary_expression_starter_set;
static ncl_symset multiplicative_expression_starter_set;
static ncl_symset multiplicative_expression_next_set;
static ncl_symset additive_expression_starter_set;
static ncl_symset additive_expression_next_set;
static ncl_symset compare_expression_starter_set;
static ncl_symset compare_expression_next_set;
static ncl_symset unary_boolean_expression_starter_set;
static ncl_symset binary_boolean_expression_starter_set;
static ncl_symset binary_boolean_expression_next_set;
static ncl_symset expression_starter_set;
static ncl_symset simple_statement_starter_set;
static ncl_symset statement_starter_set;
static ncl_symset statement_finalizer_set;
static ncl_symset statements_starter_set;
static ncl_symset statements_follower_set;

static void dynamic_initialization()
{
    basic_expression_starter_set = ncl_symset_singleton(ncl_number_tk);
    basic_expression_starter_set = ncl_symset_add_elem(basic_expression_starter_set, ncl_id_tk);
    basic_expression_starter_set = ncl_symset_add_elem(basic_expression_starter_set, ncl_string_tk);
    basic_expression_starter_set = ncl_symset_add_elem(basic_expression_starter_set, ncl_openpar_tk);
    basic_expression_closepar_set = ncl_symset_singleton(ncl_closepar_tk);

    postfix_expression_starter_set = basic_expression_starter_set;
    postfix_expression_next_set = ncl_symset_singleton(ncl_dot_tk);
    postfix_expression_next_set = ncl_symset_add_elem(postfix_expression_next_set, ncl_openpar_tk);
    postfix_expression_incall_set = ncl_symset_add_elem(basic_expression_closepar_set, ncl_comma_tk);

    unary_call_expression_starter_set = postfix_expression_starter_set;

    unary_expression_starter_set = ncl_symset_add_elem(unary_call_expression_starter_set, ncl_plus_tk);
    unary_expression_starter_set = ncl_symset_add_elem(unary_expression_starter_set, ncl_minus_tk);

    multiplicative_expression_starter_set = unary_expression_starter_set;
    multiplicative_expression_next_set = ncl_symset_singleton(ncl_mult_tk);
    multiplicative_expression_next_set = ncl_symset_add_elem(multiplicative_expression_next_set, ncl_div_tk);
    multiplicative_expression_next_set = ncl_symset_add_elem(multiplicative_expression_next_set, ncl_idiv_kw);
    multiplicative_expression_next_set = ncl_symset_add_elem(multiplicative_expression_next_set, ncl_mod_kw);
    multiplicative_expression_next_set = ncl_symset_add_elem(multiplicative_expression_next_set, ncl_rem_kw);

    additive_expression_starter_set = multiplicative_expression_starter_set;
    additive_expression_next_set = ncl_symset_singleton(ncl_plus_tk);
    additive_expression_next_set = ncl_symset_add_elem(additive_expression_next_set, ncl_minus_tk);
    additive_expression_next_set = ncl_symset_add_elem(additive_expression_next_set, ncl_ampersand_tk);

    compare_expression_starter_set = additive_expression_starter_set;
    compare_expression_next_set = ncl_symset_singleton(ncl_eq_tk);
    compare_expression_next_set = ncl_symset_add_elem(compare_expression_next_set, ncl_ne_tk);
    compare_expression_next_set = ncl_symset_add_elem(compare_expression_next_set, ncl_lt_tk);
    compare_expression_next_set = ncl_symset_add_elem(compare_expression_next_set, ncl_le_tk);
    compare_expression_next_set = ncl_symset_add_elem(compare_expression_next_set, ncl_ge_tk);
    compare_expression_next_set = ncl_symset_add_elem(compare_expression_next_set, ncl_gt_tk);

    unary_boolean_expression_starter_set = ncl_symset_add_elem(compare_expression_starter_set, ncl_not_kw);

    binary_boolean_expression_starter_set = unary_boolean_expression_starter_set;
    binary_boolean_expression_next_set = ncl_symset_singleton(ncl_and_kw);
    binary_boolean_expression_next_set = ncl_symset_add_elem(binary_boolean_expression_next_set, ncl_or_kw);

    expression_starter_set = binary_boolean_expression_starter_set;

    simple_statement_starter_set = ncl_symset_add_elem(expression_starter_set, ncl_pass_kw);
    simple_statement_starter_set = ncl_symset_add_elem(simple_statement_starter_set, ncl_exit_kw);
    simple_statement_starter_set = ncl_symset_add_elem(simple_statement_starter_set, ncl_next_kw);

    statement_starter_set = simple_statement_starter_set;
    statement_finalizer_set = ncl_symset_singleton(ncl_eol_tk);
    statement_finalizer_set = ncl_symset_add_elem(statement_finalizer_set, ncl_semicolon_tk);
    statement_finalizer_set = ncl_symset_add_elem(statement_finalizer_set, ncl_eof_tk);

    statements_starter_set = statement_starter_set;
    statements_follower_set = ncl_symset_singleton(ncl_eof_tk);

    dynamic_initialization_done = true;
}

static void error_func(ncl_lexer *lexer, char const *msg)
{
    printf("\nError line %d:\n", lexer->line_number);
    char const* sol = lexer->current_start;
    while (sol != lexer->buffer_start && sol[-1] != 0x0A && sol[-1] != 0x0D) {
        --sol;
    }
    for (char const* cur = sol; cur != lexer->buffer_end && *cur != 0x0A && *cur != 0x0D; ++cur) {
        putchar(*cur);
    }
    putchar('\n');
    for (char const* cur = sol; cur != lexer->current_start; ++cur) {
        putchar(*cur == '\t' ? '\t' : ' ');
    }
    putchar('^');
    for (char const* cur = lexer->current_start+1; cur < lexer->current_end-1; ++cur) {
        putchar('~');
    }
    if (lexer->current_end > lexer->current_start+1) {
        putchar('^');
    }
    printf("\n('%.*s': %s) %s\n", (int)(lexer->current_end-lexer->current_start), lexer->current_start,  ncl_token_names[lexer->current_kind], msg);
}

static ncl_token_kind look_ahead(ncl_lexer *lexer, bool skip_eol)
{
    ncl_lexer temp_lexer = *lexer;
    ncl_lex(&temp_lexer, skip_eol);
    return temp_lexer.current_kind;
}

static void skip_to(ncl_lexer *lexer, struct ncl_symset sync)
{
    bool skip_eol = !ncl_symset_has_elem(sync, ncl_eol_tk);
    bool skipped = false;
    while (!ncl_symset_has_elem(sync, lexer->current_kind)) {
        ncl_lex(lexer, skip_eol);
        skipped = true;
    }
    if (skipped) {
        lexer->error_func(lexer, "parsing restarted here");
    }
}

static bool ensure(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (ncl_symset_has_elem(valid, lexer->current_kind)) {
        return true;
    } else {
        lexer->error_func(lexer, msg);
        skip_to(lexer, sync);
        return ncl_symset_has_elem(valid, lexer->current_kind);
    }
}

static ncl_parse_result parse_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg);

static ncl_parse_result parse_basic_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, basic_expression_starter_set, sync, "can't start a basic expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_parse_result result;
    ncl_symset next_sync;
    bool skip = true;
    result.error = ncl_parse_ok;
    result.top = NULL;
    switch (lexer->current_kind) {
        case ncl_number_tk:
            result.top = malloc(sizeof *result.top);
            result.top->kind = ncl_number_node;
            result.top->token.start = lexer->current_start;
            result.top->token.end = lexer->current_end;
            break;
        case ncl_id_tk:
            result.top = malloc(sizeof *result.top);
            result.top->kind = ncl_id_node;
            result.top->token.start = lexer->current_start;
            result.top->token.end = lexer->current_end;
            break;
        case ncl_string_tk:
            result.top = malloc(sizeof *result.top);
            result.top->kind = ncl_string_node;
            result.top->token.start = lexer->current_start;
            result.top->token.end = lexer->current_end;
            break;
        case ncl_openpar_tk:
            ncl_lex(lexer, true);
            next_sync = ncl_symset_or(basic_expression_closepar_set, sync);
            next_sync = ncl_symset_remove_elem(next_sync, ncl_eol_tk);
            result = parse_expression(lexer, basic_expression_closepar_set, next_sync, "expecting closing parenthesis");
            skip = lexer->current_kind == ncl_closepar_tk;
            break;
    }
    if (skip) {
        ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
    }
    if (ensure(lexer, valid, sync, msg)) {
        return result;
    } else {
        result.error = ncl_parse_error;
        return result;
    }
}

static ncl_parse_result parse_postfix_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, postfix_expression_starter_set, sync, "can't start a postfix expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_symset next_valid = ncl_symset_or(postfix_expression_next_set, valid);
    ncl_parse_result result = parse_basic_expression(lexer, next_valid, sync, msg);
    while (ncl_symset_has_elem(postfix_expression_next_set, lexer->current_kind)) {
        ncl_symset next_sync;
        ncl_node *node = malloc(sizeof *node);
        ncl_node *last = NULL;
        switch (lexer->current_kind) {
            case ncl_dot_tk:
                ncl_lex(lexer, true);
                next_sync = ncl_symset_or(sync, ncl_symset_singleton(ncl_id_tk));
                if (!ensure(lexer, ncl_symset_singleton(ncl_id_tk), next_sync, "identifier expected")) {
                    result.error = ncl_parse_error;
                    return result;
                }
                node->kind = ncl_field_node;
                node->field.exp = result.top;
                node->field.start = lexer->current_start;
                node->field.end = lexer->current_end;
                result.top = node;
                ncl_lex(lexer, !ncl_symset_has_elem(next_valid, ncl_eol_tk));
                break;
            case ncl_openpar_tk:
                node->kind = ncl_call_node;
                node->call.func = result.top;
                node->call.args = NULL;
                result.top = node;
                last = NULL;
                if (look_ahead(lexer, true) == ncl_closepar_tk) {
                    ncl_lex(lexer, true);
                } else {
                    do {
                        ncl_lex(lexer, true);
                        next_sync = ncl_symset_or(basic_expression_closepar_set, sync);
                        next_sync = ncl_symset_remove_elem(next_sync, ncl_eol_tk);
                        ncl_parse_result arg = parse_expression(lexer, postfix_expression_incall_set, next_sync, "expecting closing parent");
                        if (arg.error == ncl_parse_error) {
                            result.error = ncl_parse_error;
                        }
                        node = malloc(sizeof *node);
                        node->kind = ncl_args_node;
                        node->list.head = arg.top;
                        node->list.tail = NULL;
                        if (last != NULL) {
                            last->list.tail = node;
                        } else {
                            result.top->call.args = node;
                        }
                        last = node;
                    } while (lexer->current_kind == ncl_comma_tk);
                }
                if (lexer->current_kind == ncl_closepar_tk) {
                    ncl_lex(lexer, !ncl_symset_has_elem(next_valid, ncl_eol_tk));
                }
                break;
        }
    }
    if (ensure(lexer, valid, sync, msg)) {
        return result;
    } else {
        result.error = ncl_parse_error;
        return result;
    }
}

static ncl_parse_result parse_unary_call_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, unary_call_expression_starter_set, sync, "can't start an unary call expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_symset next_valid = ncl_symset_or(postfix_expression_starter_set, valid);
    ncl_parse_result result = parse_postfix_expression(lexer, next_valid, sync, msg);
    ncl_node *last = NULL;
    while (ncl_symset_has_elem(postfix_expression_starter_set, lexer->current_kind)) {
        ncl_parse_result arg = parse_postfix_expression(lexer, next_valid, sync, msg);
        ncl_node *node = malloc(sizeof *node);
        node->kind = ncl_call1_node;
        node->call.args = arg.top;
        if (last == NULL) {
            node->call.func = result.top;
            result.top = node;
        } else {
            node->call.func = last->call.args;
            last->call.args = node;
        }
        last = node;
    }
    return result;
}

static ncl_parse_result parse_unary_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, unary_expression_starter_set, sync, "can't start an unary expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    if (lexer->current_kind != ncl_plus_tk && lexer->current_kind != ncl_minus_tk) {
        return parse_unary_call_expression(lexer, valid, sync, msg);
    } else {
        ncl_token_kind oper = lexer->current_kind;
        ncl_lex(lexer, true);
        ncl_parse_result result = parse_unary_expression(lexer, valid, sync, msg);
        ncl_node *node = malloc(sizeof *node);
        node->kind = ncl_unary_node;
        node->unary.op = oper;
        node->unary.arg = result.top;
        result.top = node;
        return result;
    }
}

static ncl_parse_result parse_multiplicative_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, multiplicative_expression_starter_set, sync, "can't start a multiplicative expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_symset next_valid = ncl_symset_or(valid, multiplicative_expression_next_set);
    ncl_symset next_sync = ncl_symset_or(sync, multiplicative_expression_next_set);
    ncl_parse_result result = parse_unary_expression(lexer, next_valid, next_sync, msg);

    while (ncl_symset_has_elem(multiplicative_expression_next_set, lexer->current_kind)) {
        ncl_token_kind oper = lexer->current_kind;
        ncl_lex(lexer, true);
        ncl_parse_result arg = parse_unary_expression(lexer, next_valid, next_sync, msg);
        ncl_node *node = malloc(sizeof *node);
        node->kind = ncl_binary_node;
        node->binary.op = oper;
        node->binary.left = result.top;
        node->binary.right = arg.top;
        result.top = node;
        if (arg.error == ncl_parse_error) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_additive_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, additive_expression_starter_set, sync, "can't start an additive expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_symset next_valid = ncl_symset_or(valid, additive_expression_next_set);
    ncl_symset next_sync = ncl_symset_or(sync, additive_expression_next_set);
    ncl_parse_result result = parse_multiplicative_expression(lexer, next_valid, next_sync, msg);

    while (ncl_symset_has_elem(additive_expression_next_set, lexer->current_kind)) {
        ncl_token_kind oper = lexer->current_kind;
        ncl_lex(lexer, true);
        ncl_parse_result arg = parse_multiplicative_expression(lexer, next_valid, next_sync, msg);
        ncl_node *node = malloc(sizeof *node);
        node->kind = ncl_binary_node;
        node->binary.op = oper;
        node->binary.left = result.top;
        node->binary.right = arg.top;
        result.top = node;
        if (arg.error == ncl_parse_error) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_compare_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, compare_expression_starter_set, sync, "can't start a compare expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_symset next_valid = ncl_symset_or(valid, compare_expression_next_set);
    ncl_symset next_sync = ncl_symset_or(sync, compare_expression_next_set);
    ncl_parse_result result = parse_additive_expression(lexer, next_valid, next_sync, msg);

    while (ncl_symset_has_elem(compare_expression_next_set, lexer->current_kind)) {
        ncl_token_kind oper = lexer->current_kind;
        ncl_lex(lexer, true);
        ncl_parse_result arg = parse_additive_expression(lexer, next_valid, next_sync, msg);
        ncl_node *node = malloc(sizeof *node);
        node->kind = ncl_binary_node;
        node->binary.op = oper;
        node->binary.left = result.top;
        node->binary.right = arg.top;
        result.top = node;
        if (arg.error == ncl_parse_error) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_unary_boolean_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, unary_boolean_expression_starter_set, sync, "can't start an unary boolean expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    if (lexer->current_kind != ncl_not_kw) {
        return parse_compare_expression(lexer, valid, sync, msg);
    } else {
        ncl_token_kind oper = lexer->current_kind;
        ncl_lex(lexer, true);
        ncl_parse_result result = parse_unary_boolean_expression(lexer, valid, sync, msg);
        ncl_node *node = malloc(sizeof *node);
        node->kind = ncl_unary_node;
        node->unary.op = oper;
        node->unary.arg = result.top;
        result.top = node;
        return result;
    }
}

static ncl_parse_result parse_binary_boolean_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, binary_boolean_expression_starter_set, sync, "can't start a binary boolean expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_symset next_valid = ncl_symset_or(valid, binary_boolean_expression_next_set);
    ncl_symset next_sync = ncl_symset_or(sync, binary_boolean_expression_next_set);
    ncl_parse_result result = parse_unary_boolean_expression(lexer, next_valid, next_sync, msg);

    while (ncl_symset_has_elem(binary_boolean_expression_next_set, lexer->current_kind)) {
        ncl_token_kind oper = lexer->current_kind;
        ncl_lex(lexer, true);
        ncl_parse_result arg = parse_unary_boolean_expression(lexer, next_valid, next_sync, msg);
        ncl_node *node = malloc(sizeof *node);
        node->kind = ncl_binary_node;
        node->binary.op = oper;
        node->binary.left = result.top;
        node->binary.right = arg.top;
        result.top = node;
        if (arg.error == ncl_parse_error) {
            result.error == ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, expression_starter_set, sync, "can't start an  expression")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    return parse_binary_boolean_expression(lexer, valid, sync, msg);
}

static ncl_parse_result parse_simple_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, simple_statement_starter_set, sync, "can't start a simple statement")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_parse_result result;
    ncl_node *node;
    ncl_symset next_sync;
    result.error = ncl_parse_ok;
    switch (lexer->current_kind) {
        case ncl_pass_kw:
            node = malloc(sizeof *node);
            node->kind = ncl_pass_node;
            result.top = node;
            ncl_lex(lexer, ncl_symset_has_elem(valid, ncl_eol_tk));
            break;
        case ncl_exit_kw:
            ncl_lex(lexer, true);
            node = malloc(sizeof *node);
            node->kind = ncl_exit_node;
            if (lexer->current_kind == ncl_id_tk) {
                node->token.start = lexer->current_start;
                node->token.end = lexer->current_end;
                ncl_lex(lexer, ncl_symset_has_elem(valid, ncl_eol_tk));
            } else {
                node->token.start = NULL;
                node->token.end = NULL;
            }
            result.top = node;
            break;
        case ncl_next_kw:
            ncl_lex(lexer, true);
            node = malloc(sizeof *node);
            node->kind = ncl_next_node;
            if (lexer->current_kind == ncl_id_tk) {
                node->token.start = lexer->current_start;
                node->token.end = lexer->current_end;
                ncl_lex(lexer, ncl_symset_has_elem(valid, ncl_eol_tk));
            } else {
                node->token.start = NULL;
                node->token.end = NULL;
            }
            result.top = node;
            break;
        default:
            return parse_expression(lexer, statement_finalizer_set, sync, msg);
    }
    if (!ensure(lexer, valid, sync, msg)) {
        result.error = ncl_parse_error;
    }
    return result;
}

static ncl_parse_result parse_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, statement_starter_set, sync, "can't start a statement")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_symset next_sync = ncl_symset_or(statement_finalizer_set, sync);
    ncl_parse_result result = parse_simple_statement(lexer, statement_finalizer_set, next_sync, "expecting semicolon");
    if (!ncl_symset_has_elem(statement_finalizer_set, lexer->current_kind)) {
        result.error = ncl_parse_error;
    } else if (lexer->current_kind == ncl_eol_tk || lexer->current_kind == ncl_semicolon_tk) {
        ncl_lex(lexer, true);
        if (!ensure(lexer, valid, sync, msg)) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_statements(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, statements_starter_set, sync, "can't start a statement")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_parse_result result = { .error = ncl_parse_none, .top = NULL };
    ncl_node *last = NULL;
    ncl_symset next_valid = ncl_symset_or(valid, statement_starter_set);
    ncl_symset next_sync = ncl_symset_or(next_valid, sync);
    while (ncl_symset_has_elem(statement_starter_set, lexer->current_kind)) {
        ncl_parse_result current = parse_statement(lexer, next_valid, next_sync, msg);
        if (result.error == ncl_parse_none) {
            result = current;
        } else {
            ncl_node *node = malloc(sizeof *node);
            node->kind = ncl_statements_node;
            node->list.head = current.top;
            node->list.tail = NULL;
            if (last == NULL) {
                ncl_node *prev = malloc(sizeof *prev);
                prev->kind = ncl_statements_node;
                prev->list.head = result.top;
                prev->list.tail = node;
                result.top = prev;
            } else {
                last->list.tail = node;
            }
            last = node;
            if (current.error == ncl_parse_error) {
                result.error = current.error;
            }
        }
    }
    return result;
}

ncl_parse_result ncl_parse(char const *current)
{
    if (!dynamic_initialization_done) {
        dynamic_initialization();
    }
    ncl_lexer lexer;
    lexer.buffer_start = current;
    lexer.buffer_pos = current;
    lexer.buffer_end = current + strlen(current);
    lexer.line_number = 1;
    lexer.error_func = error_func;
    ncl_lex(&lexer, false);
    if (ncl_symset_has_elem(statements_starter_set, lexer.current_kind)) {
        return parse_statements(&lexer, statements_follower_set, statements_follower_set, "trailing data");
    } else {
        return (ncl_parse_result) { .error = ncl_parse_none, .top = NULL };
    }
}

void ncl_free_node(ncl_node *top)
{
    switch (top->kind) {
        case ncl_error_node:
        case ncl_number_node:
        case ncl_id_node:
        case ncl_string_node:
            break;
        case ncl_statements_node:
            ncl_free_node(top->list.head);
            for (ncl_node *cur = top->list.tail, *next; cur != NULL; cur = next) {
                next = cur->list.tail;
                ncl_free_node(cur->list.head);
                free(cur);
            }
            break;
        case ncl_field_node:
            ncl_free_node(top->field.exp);
            break;
        case ncl_call_node:
        case ncl_call1_node:
            ncl_free_node(top->call.func);
            if (top->call.args != NULL) {
                ncl_free_node(top->call.args);
            }
            break;
        case ncl_args_node:
            for (ncl_node *cur = top->list.tail, *next; cur != NULL; cur = next) {
                next = cur->list.tail;
                ncl_free_node(cur->list.head);
                free(cur);
            }
            break;
        case ncl_unary_node:
            ncl_free_node(top->unary.arg);
            break;
        case ncl_binary_node:
            ncl_free_node(top->binary.left);
            ncl_free_node(top->binary.right);
            break;
    }
    free(top);
}