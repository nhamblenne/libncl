/* =======================================================================
 * test3.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdio.h>
#include "ncl_lexer.h"

void error_func(ncl_lexer *lexer, char const *msg)
{
    printf("Error: %s at %.*s\n", msg, (int)(lexer->current_end-lexer->current_start), lexer->current_start);
}

int main()
{
    char const text[] = " id1 id2 \"string1\" \"string \\\" string\"\n"
                        "# comment\n"
                        "id56$__67 $\"istring$( )$djfkl$( )$dfjlk\" 123 123.34#123E+65\n"
                        "+ - +- (: :) [ ] [:\n"
                        "\"Unterminated string";
    ncl_token_kind tk;
    ncl_lexer lexer;
    lexer.buffer_start = text;
    lexer.buffer_pos = text;
    lexer.buffer_end = text + sizeof(text);
    lexer.error_func = error_func;
    while (tk = ncl_lex(&lexer,true),  tk != ncl_eof_tk) {
        printf("%d, |%.*s|\n", tk, (int)(lexer.current_end - lexer.current_start), lexer.current_start);
        lexer.buffer_pos = lexer.current_end;
    }
    return 0;
}