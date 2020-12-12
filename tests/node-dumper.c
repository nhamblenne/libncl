/* =======================================================================
 * node-dumper.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "node-dumper.h"

#include <stdio.h>
#include "ncl_parser.h"

void show_node(int indent, ncl_node *top)
{
    if (top == NULL) {
        printf("%*s<!!! NULL !!!>\n", indent, "");
    } else {
        int next_indent = indent + 4;
        switch (top->kind) {
            case ncl_error_node:
                printf("%*sERROR\n", indent, "");
                break;
            case ncl_number_node:
                printf("%*sNUMBER %.*s\n", indent, "", (int)(top->u.token.end-top->u.token.start), top->u.token.start);
                break;
            case ncl_id_node:
                printf("%*sIDENT %.*s\n", indent, "", (int)(top->u.token.end-top->u.token.start), top->u.token.start);
                break;
            case ncl_string_node:
                printf("%*sSTRING %.*s\n", indent, "", (int)(top->u.token.end-top->u.token.start), top->u.token.start);
                break;
            case ncl_field_node:
                printf("%*sFIELD %.*s OF\n", indent, "", (int)(top->u.field.end-top->u.field.start), top->u.field.start);
                show_node(indent + 4, top->u.field.exp);
                break;
            case ncl_call_node:
                printf("%*sCALL\n", indent, "");
                show_node(indent + 4, top->u.call.func);
                for (ncl_node *cur = top->u.call.args; cur != NULL; cur = cur->u.list.tail) {
                    show_node(indent + 4, cur->u.list.head);
                }
                break;
            case ncl_call1_node:
                printf("%*sCALL1\n", indent, "");
                show_node(indent + 4, top->u.call.func);
                show_node(indent + 4, top->u.call.args);
                break;
            case ncl_unary_node:
                printf("%*sOPER %s\n", indent, "", ncl_token_names[top->u.unary.op]);
                show_node(indent + 4, top->u.unary.arg);
                break;
            case ncl_binary_node:
                printf("%*sOPER %s\n", indent, "", ncl_token_names[top->u.binary.op]);
                show_node(indent + 4, top->u.binary.left);
                show_node(indent + 4, top->u.binary.right);
                break;
            case ncl_pass_node:
                printf("%*sPASS\n", indent, "");
                break;
            case ncl_exit_node:
                printf("%*sEXIT\n", indent, "");
                if (top->u.token.start != top->u.token.end) {
                    printf("%*s%.*s\n", indent + 4, "", (int)(top->u.token.end-top->u.token.start), top->u.token.start);
                }
                break;
            case ncl_next_node:
                printf("%*sNEXT\n", indent, "");
                if (top->u.token.start != top->u.token.end) {
                    printf("%*s%.*s\n", indent + 4, "", (int)(top->u.token.end-top->u.token.start), top->u.token.start);
                }
                break;
            case ncl_return_node:
                printf("%*sRETURN\n", indent, "");
                if (top->u.exp.exp != NULL) {
                    show_node(indent + 4, top->u.exp.exp);
                }
                break;
            case ncl_assign_node:
                printf("%*sASSIGN\n", indent, "");
                show_node(indent + 4, top->u.assign.to);
                printf("%*s  :=\n", indent, "");
                show_node(indent + 4, top->u.assign.what);
                break;
            case ncl_list_node:
                if (top->u.list.tail != NULL) {
                    printf("%*sLIST\n", indent, "");
                } else {
                    next_indent = indent;
                }
                for (ncl_node *cur = top; cur != NULL; cur = cur->u.list.tail) {
                    show_node(next_indent, cur->u.list.head);
                }
                break;
            case ncl_cond_node:
                printf("%*sIF\n", indent, "");
                show_node(indent+4, top->u.cond.cond);
                printf("%*s  THEN\n", indent, "");
                show_node(indent+4, top->u.cond.then_stmt);
                printf("%*s  ELSE\n", indent, "");
                show_node(indent+4, top->u.cond.else_stmt);
                break;
            case ncl_loop_node:
                printf("%*sLOOP\n", indent, "");
                for (ncl_node *cur = top->u.block.block; cur != NULL; cur = cur->u.list.tail) {
                    show_node(indent+4, cur->u.list.head);
                }
                break;
            case ncl_while_node:
                printf("%*sWHILE\n", indent, "");
                show_node(indent+4, top->u.while_stmt.cond);
                printf("%*s  LOOP\n", indent, "");
                show_node(indent + 4, top->u.while_stmt.block);
                break;
            case ncl_for_node:
                printf("%*sFOR\n", indent, "");
                show_node(indent + 4, top->u.for_stmt.var);
                printf("%*s  IN\n", indent, "");
                show_node(indent + 4, top->u.for_stmt.exp);
                printf("%*s  LOOP\n", indent, "");
                show_node(indent + 4, top->u.for_stmt.block);
                break;
            case ncl_label_node:
                printf("%*sLABEL %.*s\n", indent, "", (int)(top->u.token.end-top->u.token.start), top->u.token.start);
                break;
            case ncl_flambda_node:
            case ncl_plambda_node:
                printf("%*sLAMBDA %s\n", indent, "", (top->kind == ncl_flambda_node ? "FUNC" : "PROC"));
                show_node(indent + 4, top->u.lambda.args);
                printf("%*s  RESULT\n", indent, "");
                show_node(indent + 4, top->u.lambda.content);
                break;
            default:
                printf("%*s<!!! UNKNOWN %d !!!>\n", indent, "", top->kind);
                break;
        }
    }
}
