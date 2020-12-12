/* =======================================================================
 * test-lexer.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

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
    printf("\n%s\n\n", msg);
}

int main(int argc, char* argv[])
{
    int status = EXIT_SUCCESS;
    size_t size = 1;
    char* buffer = malloc(size);
    if (buffer == NULL) {
        fprintf(stderr, "Unable to allocate read buffer.\n");
        return EXIT_FAILURE;
    }
    bool skip_eol = false;
    int start_arg = 1;
    if (argc > 1) {
        skip_eol = strcmp(argv[1], "-skip-eol") == 0;
    }
    for (int i = 1 + skip_eol; i < argc; ++i) {
        errno = 0;
        FILE *f = fopen(argv[i], "r");
        if (f == NULL) {
            int err = errno;
            fprintf(stderr, "Unable to open %s: %s\n", argv[i], strerror(err));
            status = EXIT_FAILURE;
            continue;
        }
        size_t nread;
        size_t end = 0;
        while (nread = fread(buffer + end, 1, size - end, f), nread > 0) {
            end += nread;
            if (end == size) {
                char* new_buffer = realloc(buffer,2*size);
                if (new_buffer == NULL) {
                    fprintf(stderr, "Unable to increase read buffer's size.\n");
                    return EXIT_FAILURE;
                }
                buffer = new_buffer;
                size += size;
            }
        }

        ncl_token_kind tk;
        ncl_lexer lexer;
        lexer.buffer_start = buffer;
        lexer.buffer_pos = buffer;
        lexer.buffer_end = buffer + end;
        lexer.line_number = 1;
        lexer.error_func = error_func;
        while (tk = ncl_lex(&lexer, skip_eol), tk != ncl_eof_tk) {
            if (tk == ncl_eol_tk) {
                printf("%-10s\n", ncl_token_names[tk]);
            } else {
                printf("%-10s => |%.*s|\n", ncl_token_names[tk], (int) (lexer.current_end - lexer.current_start),
                       lexer.current_start);
            }
            lexer.buffer_pos = lexer.current_end;
        }
    }
    free(buffer);
    return status;
}