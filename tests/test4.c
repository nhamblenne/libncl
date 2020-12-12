/* =======================================================================
 * test4.cpp.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdio.h>

#include "ncl_parser.h"
#include "node-dumper.h"

int main() {
    ncl_parse_result result;

    result = ncl_parse("exp := 36\n"
                       "exp := 42;exp := 96\n"
                       "exp := 102;exp := ab;exp := $foo;$;exp := \"toto\";"
                          "exp := (a);exp := ((a));exp := a.c;exp := (a).c.x;"
                          "exp := sin x; exp := sin x(2); exp := x(sin y);exp :=  sin cos x\n"
                       "exp := a();exp := a(b);exp :=  a(b,c);exp := a(b.c,f());"
                          "exp := +a;exp :=  -b;exp :=  sin(-x);exp := + + - a;exp :=  +f g;exp :=  (+f) g\n"
                       "exp := a\n"
                       ".b\n"
                       "exp := a\\ \n"
                       ".b;"
                          "exp := a mod +e;"
                          "exp := sin a * b / c div d mod +e rem x(x) * cos y;"
                          "exp := a + b * a + b;exp :=  (a + b) * (a + b);exp :=  a*a + 2*a*b + b*b;"
                          "exp := sin x * k < a + cos theta == foo(bar, a+b);\n"
                       "exp := not a < b*c == d;"
                           "exp := not a and not b or c == d;",
                       0);
    show_node(0, result.top);
    ncl_free_node(result.top);
}
