#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

bool division_io_read_all_bytes_from_file(const char* path, void** out_data, size_t* out_data_byte_count);

#ifdef __cplusplus
}
#endif