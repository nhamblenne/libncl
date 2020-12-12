/* =======================================================================
 * file-reader.c
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include "file-reader.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_file(char const *name, char **buffer, size_t *size, size_t *file_size) {
    errno = 0;
    FILE *f = fopen(name, "r");
    if (f == NULL) {
        int err = errno;
        fprintf(stderr, "Unable to open %s: %s\n", name, strerror(err));
        return EXIT_FAILURE;
    }
    size_t nread;
    *file_size = 0;
    while (nread = fread(*buffer + *file_size, 1, *size - *file_size, f), nread > 0) {
        *file_size += nread;
        if (*file_size + 1 >= *size) {
            char *new_buffer = realloc(buffer, 2 * *size);
            if (new_buffer == NULL) {
                fprintf(stderr, "Unable to increase read buffer's size.\n");
                return EXIT_FAILURE;
            }
            *buffer = new_buffer;
            *size += *size;
        }
    }
    (*buffer)[*file_size] = '\0';
    return EXIT_SUCCESS;
}
