#pragma once

#include <stdint.h>

typedef uint32_t DivisionId;

typedef struct DivisionIdWithBinding
{
    uint32_t id;
    uint32_t shader_location;
} DivisionIdWithBinding;