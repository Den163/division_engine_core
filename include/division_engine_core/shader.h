#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "division_engine_core/context.h"
#include <division_engine_core/data_structures/unordered_id_table.h>
#include <division_engine_core_export.h>

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

typedef struct DivisionShaderSystemContext
{
    DivisionUnorderedIdTable id_table;
    struct DivisionShaderInternal_* shaders_impl;
    size_t shader_count;
} DivisionShaderSystemContext;

bool division_engine_shader_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);
void division_engine_shader_system_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT bool division_engine_shader_program_alloc(
        DivisionContext* ctx,
        const DivisionShaderSourceDescriptor* descriptors,
        int32_t descriptor_count,
        uint32_t* out_shader_program_id
    );

    DIVISION_EXPORT void division_engine_shader_program_free(
        DivisionContext* ctx, uint32_t shader_program_id
    );

#ifdef __cplusplus
}
#endif