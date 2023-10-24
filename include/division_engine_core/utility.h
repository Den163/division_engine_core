#pragma once

#include <stdbool.h>

#define DIVISION_MIN(x, y) ((x) < (y) ? (x) : (y))
#define DIVISION_MAX(x, y) ((x) > (y) ? (x) : (y))
#define DIVISION_SWAP(TYPE, x, y) TYPE tmp = (x); (x) = (y); (y) = tmp

static inline bool division_mask_has_flag(int mask, int flag)
{
    return (mask & flag) == flag;
}
