/* =======================================================================
 * ncl_parser.c
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
static ncl_symset unary_call_expression_starter_set;
static ncl_symset expression_starter_set;
static ncl_symset statement_starter_set;
static ncl_symset statement_finalizer_set;
static ncl_symset statements_starter_set;
static ncl_symset statements_follower_set;

static void dynamic_initialization()
{
    basic_expression_starter_set = ncl_symset_add_elem(ncl_init_symset(), ncl_number_tk);

    unary_call_expression_starter_set = basic_expression_starter_set;

    expression_starter_set = unary_call_expression_starter_set;

    statement_starter_set = expression_starter_set;
    statement_finalizer_set = ncl_symset_add_elem(ncl_init_symset(), ncl_eol_tk);
    statement_finalizer_set = ncl_symset_add_elem(statement_finalizer_set, ncl_semicolon_tk);
    statement_finalizer_set = ncl_symset_add_elem(statement_finalizer_set, ncl_eof_tk);

    statements_starter_set = statement_starter_set;
    statements_follower_set = ncl_symset_add_elem(ncl_init_symset(), ncl_eof_tk);

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
    printf("\n%s\n", msg);
}

static void skip_to(ncl_lexer *lexer, struct ncl_symset sync)
{
    bool skip_EOL = !ncl_symset_has_elem(sync, ncl_eol_tk);
    bool skipped = false;
    while (!ncl_symset_has_elem(sync, lexer->current_kind)) {
        ncl_lex(lexer, skip_EOL);
        skipped = true;
    }
    if (skipped) {
        lexer->error_func(lexer, "parsing restarted here");
    }
}

static ncl_parse_result parse_basic_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync)
{
    assert(ncl_symset_has_elem(basic_expression_starter_set, lexer->current_kind));
    ncl_parse_result result;
    result.error = ncl_parse_error;
    result.top = NULL;
    switch (lexer->current_kind) {
        case ncl_number_tk:
            result.top = malloc(sizeof(ncl_node));
            result.top->kind = ncl_number_node;
            result.top->number.start = lexer->current_start;
            result.top->number.end = lexer->current_end;
            break;
    }
    ncl_lex(lexer, !ncl_symset_has_elem(valid, ncl_eol_tk));
    if (!ncl_symset_has_elem(valid, lexer->current_kind)) {
        lexer->error_func(lexer, "unexpected token");
        skip_to(lexer, sync);
    }
    return result;
}

static ncl_parse_result parse_unary_call_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync)
{
    assert(ncl_symset_has_elem(unary_call_expression_starter_set, lexer->current_kind));
    return parse_basic_expression(lexer, valid, sync);
}

static ncl_parse_result parse_expression(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync)
{
    assert(ncl_symset_has_elem(expression_starter_set, lexer->current_kind));
    return parse_unary_call_expression(lexer, valid, sync);
}

static ncl_parse_result parse_statement(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync, char const* msg)
{
    assert(ncl_symset_has_elem(statement_starter_set, lexer->current_kind));
    ncl_symset next_sync = ncl_symset_or(statement_finalizer_set, sync);
    ncl_parse_result result = parse_expression(lexer, statement_finalizer_set, next_sync);
    if (!ncl_symset_has_elem(statement_finalizer_set, lexer->current_kind)) {
        result.error = ncl_parse_error;
        if (ncl_symset_has_elem(valid, lexer->current_kind)) {
            lexer->error_func(lexer, "missing semicolon");
        } else {
            lexer->error_func(lexer, "trailing data after statement");
            skip_to(lexer, sync);
        }
    } else if (lexer->current_kind == ncl_eol_tk || lexer->current_kind == ncl_semicolon_tk) {
        ncl_lex(lexer, true);
        if (!ncl_symset_has_elem(valid, lexer->current_kind)) {
            lexer->error_func(lexer, msg);
            skip_to(lexer, sync);
        }
    }
    return result;
}

static ncl_parse_result parse_statements(ncl_lexer *lexer, ncl_symset valid, ncl_symset sync)
{
    assert(ncl_symset_has_elem(statements_starter_set, lexer->current_kind));
    ncl_parse_result result = { .error = ncl_parse_none, .top = NULL };
    ncl_node *last = NULL;
    ncl_symset next_valid = ncl_symset_or(valid, statement_starter_set);
    ncl_symset next_sync = ncl_symset_or(next_valid, sync);
    assert(ncl_symset_has_elem(statement_starter_set, lexer->current_kind));
    while (ncl_symset_has_elem(statement_starter_set, lexer->current_kind)) {
        ncl_parse_result current = parse_statement(lexer, next_valid, next_sync, "statement expected");
        if (result.error == ncl_parse_none) {
            result = current;
        } else {
            ncl_node *node = malloc(sizeof(ncl_node));
            node->kind = ncl_statements_node;
            node->statements.head = current.top;
            node->statements.tail = NULL;
            if (last == NULL) {
                ncl_node *prev = malloc(sizeof(ncl_node));
                prev->kind = ncl_statements_node;
                prev->statements.head = result.top;
                prev->statements.tail = node;
                result.top = prev;
            } else {
                last->statements.tail = node;
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
    if (!ncl_symset_has_elem(statements_starter_set, lexer.current_kind)) {
        lexer.error_func(&lexer, "invalid statement start");
        skip_to(&lexer, ncl_symset_or(statements_starter_set, statements_follower_set));
    }
    if (ncl_symset_has_elem(statements_starter_set, lexer.current_kind)) {
        return parse_statements(&lexer, statements_follower_set, statements_follower_set);
    } else {
        return (ncl_parse_result) { .error = ncl_parse_none, .top = NULL };
    }
}

void ncl_free_node(ncl_node *top)
{
    switch (top->kind) {
        case ncl_error_node:
            break;
        case ncl_statements_node:
            ncl_free_node(top->statements.head);
            for (ncl_node *cur = top->statements.tail, *next; cur != NULL; cur = next) {
                next = cur->statements.tail;
                ncl_free_node(cur->statements.head);
                free(cur);
            }
            break;
    }
    free(top);
}