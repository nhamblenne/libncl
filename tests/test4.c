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
                for (ncl_node *cur = top; cur != NULL; cur = cur->statements.tail) {
                    show_node(indent + 4, cur->statements.head);
                }
                break;
        }
    }
}

int main() {
    ncl_parse_result result;

    result = ncl_parse("36\n42;96\n102");
    show_node(0, result.top);
    ncl_free_node(result.top);
}