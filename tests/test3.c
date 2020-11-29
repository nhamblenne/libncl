/* =======================================================================
 * test3.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <assert.h>
#include <stdio.h>
#include "ncl_lexer.h"

int main()
{
    char const text[] = " id1 id2 \"string1\" \"string \\\" string\"\n"
                        "# comment\n"
                        "id56$__67 $\"istring$( )$djfkl$( )$dfjlk\"";
    ncl_lexer_result lex;
    char const *cur = text;
    char const *end = text + sizeof(text);

    while (lex = ncl_lex(cur, end, true),  lex.kind != ncl_eof_tk) {
        printf("%d, |%.*s|\n", lex.kind, (int)(lex.end - lex.start), lex.start);
        cur = lex.end;
    }
    return 0;
}