#include "division_engine_core/io_utility.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool division_io_read_all_bytes_from_file(const char* path, void** out_data, size_t* out_data_byte_count)
{
    FILE* srcFile = fopen(path, "rb");
    if (!srcFile)
    {
        fprintf(stderr, "Failed to open the file `%s`\n", path);
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
        fprintf(stderr, "Failed to reading the file `%s`\n", path);
        return false;
    }

    *out_data = data;
    *out_data_byte_count = data_size;
    return true;
}

bool division_io_write_all_bytes_to_file(const char* path, void* data, size_t data_byte_count)
{
    FILE* file = fopen(path, "wt");
    if (file == NULL)
    {
        fprintf(stderr, "Failed to open the file `%s`\n", path);
        return false;
    }

    size_t result_size = fwrite(data, 1, data_byte_count, file);
    fclose(file);

    if (result_size != data_byte_count)
    {
        fprintf(stderr, "Failed to write the file `%s`\n", path);
        return false;
    }

    return true;
}
