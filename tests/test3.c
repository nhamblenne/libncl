/* =======================================================================
 * test3.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdio.h>
#include "ncl_lexer.h"

void error_func(ncl_lexer *lexer, char const *msg)
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

int main()
{
    char const text[] = " id1 id2 \"string1\" \"string \\\" string\"\n"
                        "# comment\n"
                        "id56$__67 $\"istring$( )$djfkl$( )$dfjlk\" 123 123.34#123E+65\n"
                        "+ - +- ( ) (: :) (# #) [ ] [: :] (# #) { } {: :} {# #}\n"
                        "(# #####) (#\n(# #\n"
                        "\"Unterminated string  ))) \t\n"
                        " A\"XX\" \"XX\"A 0ABC#16 0AB.DFE#16E+14\n"
                        "ABC#16\n"
                        "$\"String\" $\"String$( )$string$( )$string\"\n"
                        "$\"string\"A )$string\"X\n"
                        "\"string with $\" $\"istring with $\" $\"istring with $ $( )$istring with $ $( )$ istring with $\"\n"
                        "\"$(\"\n 12.45#ABE+5\"Foo\""
                        "\\\\ string\n"
                        "\\\\ string\n";
    ncl_token_kind tk;
    ncl_lexer lexer;
    lexer.buffer_start = text;
    lexer.buffer_pos = text;
    lexer.buffer_end = text + sizeof(text) - 1;
    lexer.line_number = 1;
    lexer.error_func = error_func;
    while (tk = ncl_lex(&lexer, false),  tk != ncl_eof_tk) {
        printf("%d, |%.*s|\n", tk, (int)(lexer.current_end - lexer.current_start), lexer.current_start);
        lexer.buffer_pos = lexer.current_end;
    }
    return 0;
}