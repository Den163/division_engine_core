#pragma once

#include <stdbool.h>

#define DIVISION_MIN(x, y) ((x) < (y) ? (x) : (y))
#define DIVISION_MAX(x, y) ((x) > (y) ? (x) : (y))
#define DIVISION_SWAP(TYPE, x, y) TYPE tmp = (x); (x) = (y); (y) = tmp

#define DIVISION_MASK_HAS_FLAG(mask, flag) ((mask & flag) == flag)
#define DIVISION_MASK_TOGGLE_BIT_WITH_VALUE(bit, value) bit ^ value
