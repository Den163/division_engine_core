#pragma once

#include <stdint.h>

typedef enum DivisionShaderType
{
    DIVISION_SHADER_VERTEX = 1,
    DIVISION_SHADER_FRAGMENT = 2
} DivisionShaderType;

typedef enum DivisionShaderVariableType
{
    DIVISION_FLOAT = 1,
    DIVISION_DOUBLE = 2,
    DIVISION_INTEGER = 3,
    DIVISION_FVEC2 = 4,
    DIVISION_FVEC3 = 5,
    DIVISION_FVEC4 = 6,
    DIVISION_FMAT4X4 = 7
} DivisionShaderVariableType;

typedef struct DivisionShaderSourceDescriptor
{
    DivisionShaderType type;
    const char* entry_point_name;
    const char* source;
    uint32_t source_size;
} DivisionShaderSourceDescriptor;