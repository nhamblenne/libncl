/* =======================================================================
 * test6.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "ncl_parser.h"
#include "node-dumper.h"

int main() {
    ncl_parse_result result;

    result = ncl_parse("pass; pass x\n"
                       "next; next 45; next l;\n"
                       "exit; exit l; exit 101\n"
                       "return; return 36 + 56; return sin x; return pass\n"
                       "a := b; a, b := b, a; a(12), b(15) := sin x, cos x\n"
                       "proc a; proc a, b; proc; proc a+b, c+d\n"
                       "func(x) a, b\n"
                       "a := b when x == d; return when not precond;\n"
                       "proc a, b, c unless foo(y)\n"
                       "if foo then bar; end\n"
                       "if foo then bar; else qux; end\n"
                       "if foo then bar; elsif cond then foo; else qux; end\n"
                       "begin\n"
                       "   foo\n"
                       "   bar\n"
                       "   if foo then\n"
                       "       bar\n"
                       "       qux\n"
                       "   end\n"
                       "end\n"
                       "loop\n"
                       "   a\n"
                       "   b := x\n"
                       "end\n"
                       "while cond loop x := x + 1; y := y - 1; end\n"
                       "for id in keys(foo) loop print id; x := x + 1; end\n"
                       "lo: loop a(x); exit lo when foo; b(y); end\n"
                       "apply array, { x => x + 2 }\n"
                       "foreach array, { x, y do\n"
                       "   foo x\n"
                       "   bar y\n"
                       "}\n",
                       0);
    show_node(0, result.top);
    ncl_free_node(result.top);
}