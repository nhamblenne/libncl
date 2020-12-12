/* =======================================================================
 * test4.cpp.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdio.h>

#include "ncl_parser.h"

void show_node(int indent, ncl_node *top)
{
    if (top == NULL) {
        printf("%*s<!!! NULL !!!>\n", indent, "");
    } else {
        switch (top->kind) {
            case ncl_error_node:
                printf("%*sERROR\n", indent, "");
                break;
            case ncl_number_node:
                printf("%*sNUMBER %.*s\n", indent, "", (int)(top->token.end-top->token.start), top->token.start);
                break;
            case ncl_id_node:
                printf("%*sIDENT %.*s\n", indent, "", (int)(top->token.end-top->token.start), top->token.start);
                break;
            case ncl_string_node:
                printf("%*sSTRING %.*s\n", indent, "", (int)(top->token.end-top->token.start), top->token.start);
                break;
            case ncl_field_node:
                printf("%*sFIELD %.*s OF\n", indent, "", (int)(top->field.end-top->field.start), top->field.start);
                show_node(indent + 4, top->field.exp);
                break;
            case ncl_call_node:
                printf("%*sCALL\n", indent, "");
                show_node(indent + 4, top->call.func);
                for (ncl_node *cur = top->call.args; cur != NULL; cur = cur->list.tail) {
                    show_node(indent + 4, cur->list.head);
                }
                break;
            case ncl_call1_node:
                printf("%*sCALL1\n", indent, "");
                show_node(indent + 4, top->call.func);
                show_node(indent + 4, top->call.args);
                break;
            case ncl_unary_node:
                printf("%*sOPER %s\n", indent, "", ncl_token_names[top->unary.op]);
                show_node(indent + 4, top->unary.arg);
                break;
            case ncl_binary_node:
                printf("%*sOPER %s\n", indent, "", ncl_token_names[top->binary.op]);
                show_node(indent + 4, top->binary.left);
                show_node(indent + 4, top->binary.right);
                break;
            case ncl_pass_node:
                printf("%*sPASS\n", indent, "");
                break;
            case ncl_exit_node:
                printf("%*sEXIT\n", indent, "");
                if (top->token.start != top->token.end) {
                    printf("%*s%.*s\n", indent + 4, "", (int)(top->token.end-top->token.start), top->token.start);
                }
                break;
            case ncl_next_node:
                printf("%*sNEXT\n", indent, "");
                if (top->token.start != top->token.end) {
                    printf("%*s%.*s\n", indent + 4, "", (int)(top->token.end-top->token.start), top->token.start);
                }
                break;
            case ncl_return_node:
                printf("%*sRETURN\n", indent, "");
                if (top->exp.exp != NULL) {
                    show_node(indent + 4, top->exp.exp);
                }
                break;
            case ncl_assign_node:
                printf("%*sASSIGN\n", indent, "");
                show_node(indent + 4, top->assign.to);
                printf("%*s  :=\n", indent, "");
                show_node(indent + 4, top->assign.what);
                break;
            case ncl_list_node:
                for (ncl_node *cur = top; cur != NULL; cur = cur->list.tail) {
                    show_node(indent, cur->list.head);
                }
                break;
            case ncl_cond_node:
                printf("%*sIF\n", indent, "");
                show_node(indent+4, top->cond.cond);
                printf("%*s  THEN\n", indent, "");
                show_node(indent+4, top->cond.then_stmt);
                printf("%*s  ELSE\n", indent, "");
                show_node(indent+4, top->cond.else_stmt);
                break;
            case ncl_loop_node:
                printf("%*sLOOP\n", indent, "");
                for (ncl_node *cur = top->block.block; cur != NULL; cur = cur->list.tail) {
                    show_node(indent+4, cur->list.head);
                }
                break;
            case ncl_while_node:
                printf("%*sWHILE\n", indent, "");
                show_node(indent+4, top->while_stmt.cond);
                printf("%*s  LOOP\n", indent, "");
                show_node(indent + 4, top->while_stmt.block);
                break;
            case ncl_for_node:
                printf("%*sFOR\n", indent, "");
                show_node(indent + 4, top->for_stmt.var);
                printf("%*s  IN\n", indent, "");
                show_node(indent + 4, top->for_stmt.exp);
                printf("%*s  LOOP\n", indent, "");
                show_node(indent + 4, top->for_stmt.block);
                break;
            case ncl_label_node:
                printf("%*sLABEL %.*s\n", indent, "", (int)(top->token.end-top->token.start), top->token.start);
                break;
            case ncl_flambda_node:
            case ncl_plambda_node:
                printf("%*sLAMBDA %s\n", indent, "", (top->kind == ncl_flambda_node ? "FUNC" : "PROC"));
                show_node(indent + 4, top->lambda.args);
                printf("%*s  RESULT\n", indent, "");
                show_node(indent + 4, top->lambda.content);
                break;
            default:
                printf("%*s<!!! UNKNOWN %d !!!>\n", indent, "", top->kind);
                break;
        }
    }
}

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
                          "exp := not a and not b or c == d;"
                       );
    show_node(0, result.top);
    ncl_free_node(result.top);
}