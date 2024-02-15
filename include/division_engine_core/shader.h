#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "context.h"
#include "types/shader.h"

#include "data_structures/unordered_id_table.h"

#include <division_engine_core_export.h>

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