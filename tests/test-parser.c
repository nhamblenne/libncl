/* =======================================================================
 * test-parser.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "ncl_parser.h"
#include "node-dumper.h"
#include "file-reader.h"

int main(int argc, char *argv[]) {
    int status = EXIT_SUCCESS;
    size_t buffer_size = 1;
    char *buffer = malloc(buffer_size);
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

        ncl_parse_result result = ncl_parse(buffer, file_size);
        show_node(0, result.top);
        ncl_free_node(result.top);
    }
    free(buffer);
    return status;
}
