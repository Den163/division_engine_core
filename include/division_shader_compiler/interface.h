#pragma once

#include <stdbool.h>

#include "division_engine_core/shader.h"
#include "division_engine_core_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum DivisionCompilerShaderType {
    DIVISION_COMPILER_SHADER_TYPE_VERTEX = 1,
    DIVISION_COMPILER_SHADER_TYPE_FRAGMENT = 2
} DivisionCompilerShaderType;

DIVISION_EXPORT bool division_shader_compiler_alloc();
DIVISION_EXPORT bool division_shader_compiler_compile_glsl_to_spirv(
    const char* source, int32_t source_size,
    DivisionCompilerShaderType shader_type,
    const char* spirv_entry_point_name,
    void** out_spirv,
    size_t* out_spirv_byte_count);

DIVISION_EXPORT bool division_shader_compiler_compile_spirv_to_metal(
    const void* spirv_bytes, size_t spirv_byte_count, DivisionCompilerShaderType shader_type, const char* entry_point,
    char** out_metal_source, size_t* out_metal_size);

DIVISION_EXPORT void division_shader_compiler_spirv_source_free(void* spirv_source);

DIVISION_EXPORT void division_shader_compiler_metal_source_free(char* metal_source);

DIVISION_EXPORT void division_shader_compiler_free();

#ifdef __cplusplus
}
#endif