#ifndef LIBNCL_FILE_READER_H
#define LIBNCL_FILE_READER_H

/* =======================================================================
 * file-reader.h
 * Copyright (c) 2020 Nicolas Ivan Hamblenne
 * =======================================================================
 */

#include <stdlib.h>

int read_file(char const *name, char **buffer, size_t *buffer_size, size_t *file_size);

#endif
