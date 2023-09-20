#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct DivisionSettings
    {
        uint32_t window_width;
        uint32_t window_height;
        const char* window_title;
    } DivisionSettings;

#ifdef __cplusplus
}
#endif