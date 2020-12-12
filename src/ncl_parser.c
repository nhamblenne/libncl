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
static ncl_symset lambda_cont_set;
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
static ncl_symset assignment_cont_set;
static ncl_symset simple_statement_starter_set;
static ncl_symset if_statement_cont_set;
static ncl_symset for_statement_cont_set;
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
    basic_expression_starter_set = ncl_symset_add_elem(basic_expression_starter_set, ncl_openbraces_tk);
    basic_expression_closepar_set = ncl_symset_singleton(ncl_closepar_tk);

    lambda_cont_set = ncl_symset_add_elem(ncl_symset_singleton(ncl_return_tk), ncl_do_kw);

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

    assignment_cont_set = ncl_symset_singleton(ncl_comma_tk);
    assignment_cont_set = ncl_symset_add_elem(assignment_cont_set, ncl_assign_tk);

    simple_statement_starter_set = ncl_symset_add_elem(expression_starter_set, ncl_pass_kw);
    simple_statement_starter_set = ncl_symset_add_elem(simple_statement_starter_set, ncl_exit_kw);
    simple_statement_starter_set = ncl_symset_add_elem(simple_statement_starter_set, ncl_next_kw);
    simple_statement_starter_set = ncl_symset_add_elem(simple_statement_starter_set, ncl_return_kw);

    if_statement_cont_set = ncl_symset_add_elem(ncl_symset_singleton(ncl_elsif_kw), ncl_else_kw);
    if_statement_cont_set = ncl_symset_add_elem(if_statement_cont_set, ncl_end_kw);

    for_statement_cont_set = ncl_symset_add_elem(ncl_symset_singleton(ncl_in_kw), ncl_loop_kw);
    for_statement_cont_set = ncl_symset_add_elem(for_statement_cont_set, ncl_end_kw);

    statement_starter_set = simple_statement_starter_set;
    statement_starter_set = ncl_symset_add_elem(statement_starter_set, ncl_if_kw);
    statement_starter_set = ncl_symset_add_elem(statement_starter_set, ncl_begin_kw);
    statement_starter_set = ncl_symset_add_elem(statement_starter_set, ncl_while_kw);
    statement_starter_set = ncl_symset_add_elem(statement_starter_set, ncl_loop_kw);
    statement_starter_set = ncl_symset_add_elem(statement_starter_set, ncl_for_kw);

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

static ncl_node *allocate_list_node(ncl_node *head, ncl_node *tail)
{
    ncl_node *result = malloc(sizeof *result);
    result->kind = ncl_list_node;
    result->list.head = head;
    result->list.tail = tail;
    return result;
}

static ncl_node *allocate_token_node(ncl_lexer *lexer)
{
    ncl_node *result = malloc(sizeof *result);
    switch (lexer->current_kind) {
        case ncl_id_tk:     result->kind = ncl_id_node; break;
        case ncl_number_tk: result->kind = ncl_number_node; break;
        case ncl_string_tk: result->kind = ncl_string_node; break;
        default:
            assert(false);
    }
    result->token.start = lexer->current_start;
    result->token.end = lexer->current_end;
    return result;
}

static ncl_node *allocate_unary_expression(ncl_token_kind op, ncl_node *arg)
{
    ncl_node *result = malloc(sizeof *result);
    result->kind = ncl_unary_node;
    result->unary.op = op;
    result->unary.arg = arg;
    return result;
}

static ncl_node *allocate_binary_expression(ncl_token_kind op, ncl_node *left, ncl_node *right)
{
    ncl_node *result = malloc(sizeof *result);
    result->kind = ncl_binary_node;
    result->binary.op = op;
    result->binary.left = left;
    result->binary.right = right;
    return result;
}

static ncl_parse_result parse_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg);
static ncl_parse_result parse_statements(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg);

static ncl_parse_result parse_id_list(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, ncl_symset_add_elem(valid, ncl_id_tk), sync, "expecting an identifier")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_parse_result result;
    result.error = ncl_parse_ok;
    result.top = NULL;
    ncl_node **last = &result.top;
    ncl_symset next_valid = ncl_symset_add_elem(valid, ncl_comma_tk);
    ncl_symset next_sync = ncl_symset_add_elem(sync, ncl_comma_tk);
    if (lexer->current_kind == ncl_id_tk) {
        ncl_node *id = allocate_token_node(lexer);
        result.top = allocate_list_node(id, NULL);
        last = &result.top->list.tail;
        ncl_lex(lexer, true);
        ensure(lexer, next_valid, next_sync, msg);
    } else {
        result.error = ncl_parse_error;
    }
    while (lexer->current_kind == ncl_comma_tk) {
        ncl_lex(lexer, true);
        ensure(lexer, ncl_symset_singleton(ncl_id_tk), next_sync, "expecting an identifier");
        if (lexer->current_kind == ncl_id_tk) {
            ncl_node *id = allocate_token_node(lexer);
            ncl_node *node = allocate_list_node(id, NULL);
            *last = node;
            last = &node->list.tail;
            ncl_lex(lexer, true);
            ensure(lexer, next_valid, next_sync, msg);
        } else {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

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
            result.top = allocate_token_node(lexer);
            break;
        case ncl_id_tk:
            result.top = allocate_token_node(lexer);
            break;
        case ncl_string_tk:
            result.top = allocate_token_node(lexer);
            break;
        case ncl_openpar_tk:
            ncl_lex(lexer, true);
            next_sync = ncl_symset_or(basic_expression_closepar_set, sync);
            next_sync = ncl_symset_remove_elem(next_sync, ncl_eol_tk);
            result = parse_expression(lexer, basic_expression_closepar_set, next_sync, "expecting closing parenthesis");
            skip = lexer->current_kind == ncl_closepar_tk;
            break;
        case ncl_openbraces_tk:
            ncl_lex(lexer, true);
            next_sync = ncl_symset_or(lambda_cont_set, sync);
            next_sync = ncl_symset_add_elem(next_sync, ncl_closebraces_tk);
            result = parse_id_list(lexer, lambda_cont_set, next_sync, "expecting do or =>");
            ncl_node *node = malloc(sizeof *node);
            node->lambda.args = result.top;
            result.top = node;
            ncl_parse_result content;
            switch (lexer->current_kind) {
                case ncl_return_tk:
                    result.top->kind = ncl_flambda_node;
                    ncl_lex(lexer, true);
                    content = parse_expression(lexer, ncl_symset_singleton(ncl_closebraces_tk), next_sync, "expecting }");
                    if (content.error == ncl_parse_error) {
                        result.error = ncl_parse_error;
                    }
                    result.top->lambda.content = content.top;
                    break;
                case ncl_do_kw:
                    node->kind = ncl_plambda_node;
                    ncl_lex(lexer, true);
                    content = parse_statements(lexer, ncl_symset_singleton(ncl_closebraces_tk), next_sync, "expecting }");
                    if (content.error == ncl_parse_error) {
                        result.error = ncl_parse_error;
                    }
                    result.top->lambda.content = content.top;
                    break;
            }
            skip = lexer->current_kind == ncl_closebraces_tk;
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
        ncl_node **last = NULL;
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
                last = &result.top->call.args;
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
                        node = allocate_list_node(arg.top, NULL);
                        *last = node;
                        last = &node->list.tail;
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
    ncl_node **last = &result.top;
    while (ncl_symset_has_elem(postfix_expression_starter_set, lexer->current_kind)) {
        ncl_parse_result arg = parse_postfix_expression(lexer, next_valid, sync, msg);
        ncl_node *node = malloc(sizeof *node);
        node->kind = ncl_call1_node;
        node->call.args = arg.top;
        node->call.func = *last;
        *last = node;
        last = &node->call.args;
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
        result.top = allocate_unary_expression(oper, result.top);
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
        result.top = allocate_binary_expression(oper, result.top, arg.top);
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
        result.top = allocate_binary_expression(oper, result.top, arg.top);
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
        result.top = allocate_binary_expression(oper, result.top, arg.top);
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
        result.top = allocate_unary_expression(oper, result.top);
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
        result.top = allocate_binary_expression(oper, result.top, arg.top);
        if (arg.error == ncl_parse_error) {
            result.error = ncl_parse_error;
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

static ncl_parse_result parse_assignation(ncl_lexer *lexer, ncl_parse_result exp, ncl_symset valid, ncl_symset sync, char const *msg)
{
    ncl_parse_result result = exp;
    ncl_symset next_sync = ncl_symset_or(assignment_cont_set, sync);
    result.top = malloc(sizeof *result.top);
    result.top->kind = ncl_assign_node;
    result.top->assign.to = allocate_list_node(exp.top, NULL);
    result.top->assign.what = NULL;
    ncl_node **last = &result.top->assign.to->list.tail;
    while (lexer->current_kind == ncl_comma_tk) {
        ncl_lex(lexer, true);
        ncl_parse_result arg = parse_expression(lexer, assignment_cont_set, next_sync, "expecting ':='");
        if (arg.error == ncl_parse_error) {
            result.error = ncl_parse_error;
        }
        ncl_node *node = allocate_list_node(arg.top, NULL);
        *last = node;
        last = &node->list.tail;
    }
    if (lexer->current_kind == ncl_assign_tk) {
        ncl_symset next_valid = ncl_symset_add_elem(valid, ncl_comma_tk);
        next_sync = ncl_symset_or(next_valid, sync);
        last = &result.top->assign.what;
        do {
            ncl_lex(lexer, true);
            ncl_parse_result arg = parse_expression(lexer, next_valid, next_sync, msg);
            if (arg.error == ncl_parse_error) {
                result.error = ncl_parse_error;
            }
            ncl_node *node = allocate_list_node(arg.top, NULL);
            *last = node;
            last = &node->list.tail;
        } while (lexer->current_kind == ncl_comma_tk);
    }
    return result;
}

static ncl_parse_result parse_proc_call(ncl_lexer *lexer, ncl_parse_result exp, ncl_symset valid, ncl_symset sync, char const *msg)
{
    ncl_parse_result result = exp;
    if (ncl_symset_has_elem(expression_starter_set, lexer->current_kind)) {
        ncl_symset next_sync = ncl_symset_add_elem(sync, ncl_comma_tk);
        next_sync = ncl_symset_or(next_sync, expression_starter_set);
        ncl_symset next_valid = ncl_symset_or(assignment_cont_set, valid);
        result.top = malloc(sizeof *result.top);
        result.top->kind = ncl_call_node;
        result.top->call.func = exp.top;
        result.top->call.args = NULL;
        ncl_node **last = &result.top->call.args;
        while (ncl_symset_has_elem(expression_starter_set, lexer->current_kind)
               || lexer->current_kind == ncl_comma_tk) {
            if (lexer->current_kind == ncl_comma_tk) {
                ncl_lex(lexer, true);
            }
            ncl_parse_result arg = parse_expression(lexer, next_valid, next_sync, msg);
            if (arg.error == ncl_parse_error) {
                result.error = ncl_parse_error;
            }
            ncl_node *node = allocate_list_node(arg.top, NULL);
            *last = node;
            last = &node->list.tail;
            if (ncl_symset_has_elem(expression_starter_set, lexer->current_kind)) {
                lexer->error_func(lexer, "expecting ,");
                result.error = ncl_parse_error;
            }
        }
    } else if (result.top->kind != ncl_call_node) {
        result.top = malloc(sizeof *result.top);
        result.top->kind = ncl_call_node;
        result.top->call.func = exp.top;
        result.top->call.args = NULL;
    }
    return result;
}

static ncl_parse_result parse_simple_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, simple_statement_starter_set, sync, "can't start a simple statement")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_parse_result result;
    ncl_node *node;
    ncl_symset next_valid;
    ncl_symset next_sync;
    bool check_valid = true;
    result.error = ncl_parse_ok;
    switch (lexer->current_kind) {
        case ncl_pass_kw:
            node = malloc(sizeof *node);
            node->kind = ncl_pass_node;
            result.top = node;
            ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
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
        case ncl_return_kw:
            ncl_lex(lexer, ncl_symset_has_elem(valid, ncl_eol_tk));
            node = malloc(sizeof *node);
            node->kind = ncl_return_node;
            if (ncl_symset_has_elem(expression_starter_set, lexer->current_kind)) {
                result = parse_expression(lexer, valid, sync, msg);
                node->exp.exp = result.top;
                check_valid = false;
            } else {
                node->exp.exp = NULL;
            }
            result.top = node;
            break;
        default:
            next_valid = ncl_symset_add_elem(valid, ncl_comma_tk);
            next_valid = ncl_symset_add_elem(next_valid, ncl_assign_tk);
            next_valid = ncl_symset_or(next_valid, expression_starter_set);
            next_sync = ncl_symset_add_elem(sync, ncl_comma_tk);
            next_sync = ncl_symset_add_elem(next_sync, ncl_assign_tk);
            result = parse_postfix_expression(lexer, next_valid, next_sync, msg);
            if (lexer->current_kind == ncl_comma_tk || lexer->current_kind == ncl_assign_tk) {
                result = parse_assignation(lexer, result, valid, sync, msg);
            } else {
                result = parse_proc_call(lexer, result, valid, sync, msg);
            }
            check_valid = false;
            break;
    }
    if (check_valid && !ensure(lexer, valid, sync, msg)) {
        result.error = ncl_parse_error;
    }
    return result;
}

static ncl_parse_result parse_if_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, ncl_symset_singleton(ncl_if_kw), sync, "expecting if")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_lex(lexer, true);
    ncl_symset next_sync = ncl_symset_add_elem(sync, ncl_then_kw);
    next_sync = ncl_symset_or(next_sync, statements_starter_set);
    next_sync = ncl_symset_add_elem(next_sync, ncl_elsif_kw);
    next_sync = ncl_symset_add_elem(next_sync, ncl_else_kw);
    ncl_parse_result result = parse_expression(lexer, ncl_symset_singleton(ncl_then_kw), next_sync, "expecting then");
    ncl_node *node = malloc(sizeof *node);
    node->kind = ncl_cond_node;
    node->cond.cond = result.top;
    node->cond.then_stmt = NULL;
    node->cond.else_stmt = NULL;
    result.top = node;
    if (lexer->current_kind == ncl_then_kw) {
        ncl_lex(lexer, true);
    }
    ncl_parse_result block = parse_statements(lexer, if_statement_cont_set, next_sync, "expecting else, elsif or end");
    if (block.error == ncl_parse_error) {
        result.error = ncl_parse_error;
    }
    node->cond.then_stmt = block.top;
    while (lexer->current_kind == ncl_elsif_kw) {
        ncl_lex(lexer, true);
        ncl_parse_result cond = parse_expression(lexer, ncl_symset_singleton(ncl_then_kw), next_sync, "expecting then");
        if (lexer->current_kind == ncl_then_kw) {
            ncl_lex(lexer, true);
        }
        if (cond.error == ncl_parse_error) {
            result.error = ncl_parse_error;
        }
        ncl_node *sub = malloc(sizeof *sub);
        sub->kind = ncl_cond_node;
        sub->cond.cond = cond.top;
        node->cond.else_stmt = sub;
        node = sub;
        block = parse_statements(lexer, if_statement_cont_set, next_sync, "expecting else, elsif or end");
        if (block.error == ncl_parse_error) {
            result.error = ncl_parse_error;
        }
        node->cond.then_stmt = block.top;
        node->cond.else_stmt = NULL;
    }
    if (lexer->current_kind == ncl_else_kw) {
        ncl_lex(lexer, true);
        block = parse_statements(lexer, if_statement_cont_set, next_sync, "expecting end");
        if (block.error == ncl_parse_error) {
            result.error = ncl_parse_error;
        }
        node->cond.else_stmt = block.top;
    }
    if (lexer->current_kind == ncl_end_kw) {
        ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
        if (!ensure(lexer, valid, sync, msg)) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_begin_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, ncl_symset_singleton(ncl_begin_kw), sync, "expecting begin")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_lex(lexer, true);
    ncl_symset next_sync = ncl_symset_add_elem(sync, ncl_end_kw);
    ncl_parse_result result = parse_statements(lexer, ncl_symset_singleton(ncl_end_kw), next_sync, "expecting end");
    if (lexer->current_kind == ncl_end_kw) {
        ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
        if (!ensure(lexer, valid, sync, msg)) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_loop_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, ncl_symset_singleton(ncl_loop_kw), sync, "expecting loop")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_lex(lexer, true);
    ncl_symset next_sync = ncl_symset_add_elem(sync, ncl_end_kw);
    ncl_parse_result result = parse_statements(lexer, ncl_symset_singleton(ncl_end_kw), next_sync, "expecting end");
    ncl_node *node = malloc(sizeof *node);
    node->kind = ncl_loop_node;
    node->block.block = result.top;
    result.top = node;
    if (lexer->current_kind == ncl_end_kw) {
        ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
        if (!ensure(lexer, valid, sync, msg)) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_while_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, ncl_symset_singleton(ncl_while_kw), sync, "expecting while")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_lex(lexer, true);
    ncl_symset next_sync = ncl_symset_add_elem(sync, ncl_loop_kw);
    next_sync = ncl_symset_or(next_sync, statements_starter_set);
    ncl_parse_result result = parse_expression(lexer, ncl_symset_singleton(ncl_loop_kw), next_sync, "expecting loop");
    ncl_node *node = malloc(sizeof *node);
    node->kind = ncl_while_node;
    node->while_stmt.cond = result.top;
    node->while_stmt.block = NULL;
    result.top = node;
    if (lexer->current_kind == ncl_loop_kw) {
        ncl_lex(lexer, true);
    }
    ncl_parse_result block = parse_statements(lexer, ncl_symset_singleton(ncl_end_kw), next_sync, "expecting end");
    if (block.error == ncl_parse_error) {
        result.error = ncl_parse_error;
    }
    node->while_stmt.block = block.top;
    if (lexer->current_kind == ncl_end_kw) {
        ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
        if (!ensure(lexer, valid, sync, msg)) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_for_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    ncl_symset next_sync = ncl_symset_or(sync, for_statement_cont_set);
    if (!ensure(lexer, ncl_symset_singleton(ncl_for_kw), sync, "expecting for")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_lex(lexer, true);
    ncl_node *node = malloc(sizeof *node);
    node->kind = ncl_for_node;
    ensure(lexer, ncl_symset_singleton(ncl_id_tk), next_sync, "expecting id" );
    ncl_node *var = malloc(sizeof *node);
    var->kind = ncl_id_node;
    var->token.start = lexer->current_start;
    var->token.end = lexer->current_end;
    node->for_stmt.var = var;

    ncl_lex(lexer, true);
    ensure(lexer, ncl_symset_singleton(ncl_in_kw), next_sync, "expecting in" );
    if (lexer->current_kind == ncl_in_kw) {
        ncl_lex(lexer, true);
    }

    ncl_parse_result result = parse_expression(lexer, ncl_symset_singleton(ncl_loop_kw), next_sync, "expecting loop");
    node->for_stmt.exp = result.top;
    result.top = node;
    if (lexer->current_kind == ncl_loop_kw) {
        ncl_lex(lexer, true);
    }
    next_sync = ncl_symset_add_elem(sync, ncl_end_kw);
    ncl_parse_result block = parse_statements(lexer, ncl_symset_singleton(ncl_end_kw), next_sync, "expecting end");
    if (block.error == ncl_parse_error) {
        result.error = ncl_parse_error;
    }
    node->for_stmt.block = block.top;
    if (lexer->current_kind == ncl_end_kw) {
        ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
        if (!ensure(lexer, valid, sync, msg)) {
            result.error = ncl_parse_error;
        }
    }
    return result;
}

static ncl_parse_result parse_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const *msg)
{
    if (!ensure(lexer, statement_starter_set, sync, "can't start a statement")) {
        return (ncl_parse_result){ .error = ncl_parse_error, .top = NULL };
    }
    ncl_symset next_sync = ncl_symset_or(statement_finalizer_set, sync);
    ncl_parse_result result;
    switch (lexer->current_kind) {
        case ncl_if_kw:
            result = parse_if_statement(lexer, statement_finalizer_set, next_sync, msg);
            break;
        case ncl_begin_kw:
            result = parse_begin_statement(lexer, statement_finalizer_set, next_sync, msg);
            break;
        case ncl_loop_kw:
            result = parse_loop_statement(lexer, statement_finalizer_set, next_sync, msg);
            break;
        case ncl_while_kw:
            result = parse_while_statement(lexer, statement_finalizer_set, next_sync, msg);
            break;
        case ncl_for_kw:
            result = parse_for_statement(lexer, statement_finalizer_set, next_sync, msg);
            break;
        case ncl_id_tk:
            if (look_ahead(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk)) == ncl_colon_tk) {
                result.error = ncl_parse_ok;
                result.top = malloc(sizeof *result.top);
                result.top->kind = ncl_label_node;
                result.top->token.start = lexer->current_start;
                result.top->token.end = lexer->current_end;
                ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
                ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
                if (!ensure(lexer, valid, sync, msg)) {
                    result.error = ncl_parse_error;
                }
                return result;
            }
        default: {
            ncl_symset next_valid = ncl_symset_add_elem(statement_finalizer_set, ncl_when_kw);
            next_valid = ncl_symset_add_elem(next_valid, ncl_unless_kw);
            result = parse_simple_statement(lexer, next_valid, next_sync, "expecting semicolon");
            ncl_parse_result arg;
            ncl_node *node;
            switch (lexer->current_kind) {
                case ncl_when_kw:
                    ncl_lex(lexer, true);
                    arg = parse_expression(lexer, statement_finalizer_set, next_sync, "expecting semicolon");
                    if (arg.error == ncl_parse_error) {
                        result.error = ncl_parse_error;
                    }
                    node = malloc(sizeof *node);
                    node->kind = ncl_cond_node;
                    node->cond.cond = arg.top;
                    node->cond.then_stmt = result.top;
                    node->cond.else_stmt = NULL;
                    result.top = node;
                    break;
                case ncl_unless_kw:
                    ncl_lex(lexer, true);
                    arg = parse_expression(lexer, statement_finalizer_set, next_sync, "expecting semicolon");
                    if (arg.error == ncl_parse_error) {
                        result.error = ncl_parse_error;
                    }
                    node = malloc(sizeof *node);
                    node->kind = ncl_cond_node;
                    node->cond.cond = arg.top;
                    node->cond.then_stmt = NULL;
                    node->cond.else_stmt = result.top;
                    result.top = node;
                    break;
            }
            break;
        }
    }
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
    ncl_parse_result result = { .error = ncl_parse_ok, .top = NULL };
    ncl_node **last = &result.top;
    ncl_symset next_valid = ncl_symset_or(valid, statement_starter_set);
    ncl_symset next_sync = ncl_symset_or(next_valid, sync);
    while (ncl_symset_has_elem(statement_starter_set, lexer->current_kind)) {
        ncl_parse_result current = parse_statement(lexer, next_valid, next_sync, msg);
        if (current.error == ncl_parse_error) {
            result.error = current.error;
        }
        ncl_node *node = allocate_list_node(current.top, NULL);
        *last = node;
        last = &node->list.tail;
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