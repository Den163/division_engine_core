#include "division_engine_core/io_utility.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool division_io_read_all_bytes_from_file(const char* path, void** out_data, size_t* out_data_byte_count)
{
    FILE* srcFile = fopen(path, "rb");
    if (!srcFile)
    {
        fprintf(stderr, "Cannot open the file `%s`\n", path);
        return false;
    }

    fseek(srcFile, 0, SEEK_END);
    size_t data_size = ftell(srcFile);
    fseek(srcFile, 0, SEEK_SET);

    void* data = malloc(sizeof(char[data_size]));
    size_t readSize = fread(data, sizeof(char), data_size, srcFile);
    fclose(srcFile);

    if (readSize != data_size)
    {
        fprintf(stderr, "Error while reading the file `%s`\n", path);
        return false;
    }

    *out_data = data;
    *out_data_byte_count = data_size;
    return true;
}