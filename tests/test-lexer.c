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
#include "file-reader.h"

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
    size_t buffer_size = 1024;
    char* buffer = malloc(buffer_size);
    if (buffer == NULL) {
        fprintf(stderr, "Unable to allocate read buffer.\n");
        return EXIT_FAILURE;
    }
    bool skip_eol = false;
    if (argc > 1) {
        skip_eol = strcmp(argv[1], "-skip-eol") == 0;
    }
    for (int i = 1 + skip_eol; i < argc; ++i) {
        size_t file_size;
        if (read_file(argv[i], &buffer, &buffer_size, &file_size) == EXIT_FAILURE) {
            status = EXIT_FAILURE;
            continue;
        }
        ncl_token_kind tk;
        ncl_lexer lexer;
        lexer.buffer_start = buffer;
        lexer.buffer_pos = buffer;
        lexer.buffer_end = buffer + file_size;
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