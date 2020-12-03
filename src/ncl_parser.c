/* =======================================================================
 * ncl_parser.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_parser.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ncl_lexer.h"

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

static void skip_to(ncl_lexer *lexer, ncl_token_kind sync1, ncl_token_kind sync2)
{
    while (lexer->current_kind != sync1
           && lexer->current_kind != sync2
           && lexer->current_kind != ncl_eof_tk)
    {
        ncl_lex(lexer, sync1 != ncl_eol_tk);
    }
}

static ncl_parse_result parse_expression(ncl_lexer *lexer)
{
    lexer->error_func(lexer, "Not a valid expression");
    skip_to(lexer, ncl_eol_tk, ncl_semicolon_tk);
    ncl_node *result = malloc(sizeof(ncl_node));
    result->kind = ncl_error_node;
    return (ncl_parse_result){ .error = ncl_parse_error, .top = result };
}

static ncl_parse_result parse_statement(ncl_lexer *lexer)
{
    ncl_parse_result result = parse_expression(lexer);
    if (lexer->current_kind != ncl_eol_tk
        && lexer->current_kind != ncl_semicolon_tk
        && lexer->current_kind != ncl_eof_tk)
    {
        result.error = ncl_parse_error;
        lexer->error_func(lexer, "Trailing data after statement");
        skip_to(lexer, ncl_eol_tk, ncl_semicolon_tk);
    }
    if (lexer->current_kind == ncl_eol_tk
        || lexer->current_kind == ncl_semicolon_tk)
    {
        ncl_lex(lexer, true);
    }
    return result;
}

static ncl_parse_result parse_statements(ncl_lexer *lexer)
{
    ncl_parse_result result = { .error = ncl_parse_none, .top = NULL };
    ncl_node *last = NULL;
    while (lexer->current_kind != ncl_eof_tk) {
        ncl_parse_result current = parse_statement(lexer);
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
    ncl_lexer lexer;
    lexer.buffer_start = current;
    lexer.buffer_pos = current;
    lexer.buffer_end = current + strlen(current);
    lexer.line_number = 1;
    lexer.error_func = error_func;
    ncl_lex(&lexer, false);
    return parse_statements(&lexer);
}

void ncl_free_node(ncl_node *top)
{
    switch (top->kind) {
        case ncl_error_node:
            break;
        case ncl_statements_node:
            for (ncl_node *cur = top->statements.head, *next; cur != NULL; cur = next) {
                ncl_free_node(cur->statements.head);
                next = cur->statements.tail;
                free(cur);
            }
            break;
    }
    free(top);
}