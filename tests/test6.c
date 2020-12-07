/* =======================================================================
 * test6.c
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
            default:
                printf("%*s<!!! UNKNOWN !!!>\n", indent, "");
                break;
        }
    }
}

int main() {
    ncl_parse_result result;

    result = ncl_parse("pass; pass x\n"
                       "next; next 45; next l;\n"
                       "exit; exit l; exit 101\n"
                       "return; return 36 + 56; return sin x; return pass\n"
    );
    show_node(0, result.top);
    ncl_free_node(result.top);
}