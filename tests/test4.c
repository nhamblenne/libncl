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
            case ncl_statements_node:
                printf("%*sSTATEMENTS\n", indent, "");
                for (ncl_node *cur = top; cur != NULL; cur = cur->list.tail) {
                    show_node(indent + 4, cur->list.head);
                }
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
                printf("%*sFIELD %.*s of\n", indent, "", (int)(top->field.end-top->field.start), top->field.start);
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
                printf("%*sCALL\n", indent, "");
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
            default:
                printf("%*s<!!! UNKNOWN !!!>\n", indent, "");
                break;
        }
    }
}

int main() {
    ncl_parse_result result;

    result = ncl_parse("36\n42;96\n102;ab;$foo;$;\"toto\";"
                       "(a);((a));a.c;(a).c.x;"
                       "sin x; sin x(2); x(sin y); sin cos x\n"
                       "a();a(b); a(b,c);a(b.c,f());"
                       "+a; -b; sin(-x);+ + - a; +f g; (+f) g\n"
                       "a\n.b\na\\ \n.b;"
                       );
    show_node(0, result.top);
    ncl_free_node(result.top);
}