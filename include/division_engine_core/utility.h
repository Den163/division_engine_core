#pragma once

#include <stdbool.h>

static inline bool division_mask_has_flag(int mask, int flag)
{
    return (mask & flag) == flag;
}
