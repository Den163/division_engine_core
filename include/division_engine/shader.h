#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "division_engine/context.h"
#include <division_engine_core_export.h>

typedef enum {
    DIVISION_SHADER_VERTEX = 0,
    DIVISION_SHADER_FRAGMENT = 1
} DivisionShaderType;

typedef enum {
    DIVISION_FLOAT = 0,
    DIVISION_DOUBLE = 1,
    DIVISION_INTEGER = 2,
    DIVISION_FVEC2 = 3,
    DIVISION_FVEC3 = 4,
    DIVISION_FVEC4 = 5,
    DIVISION_FMAT4X4 = 6
} DivisionShaderVariableType;

typedef struct DivisionShaderSettings {
    DivisionShaderType type;
    const char* file_path;
    const char* entry_point_name;
} DivisionShaderSettings;

bool division_engine_internal_shader_system_context_alloc(DivisionContext* ctx, const DivisionSettings* settings);
void division_engine_internal_shader_system_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT int32_t division_engine_shader_program_create(
    DivisionContext* ctx, const DivisionShaderSettings* settings, int32_t source_count);

DIVISION_EXPORT void division_engine_shader_program_free(DivisionContext* ctx, int32_t program_id);

#ifdef __cplusplus
}
#endif