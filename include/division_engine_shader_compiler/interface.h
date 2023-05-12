#pragma once

#include <stdbool.h>

#include "division_engine_core/shader.h"
#include "division_engine_core_export.h"

#ifdef __cplusplus
extern "C" {
#endif

DIVISION_EXPORT bool division_engine_shader_compiler_alloc();
DIVISION_EXPORT bool division_engine_shader_compiler_compile_glsl_to_spirv(
    const char* source, DivisionShaderType shader_type,
    const char* spirv_entry_point_name,
    void** out_spirv,
    size_t* out_spirv_byte_count
);
DIVISION_EXPORT void division_engine_shader_compiler_free();

#ifdef __cplusplus
}
#endif