#pragma once

#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

typedef struct DivisionMetalAttribute DivisionMetalAttribute;

typedef struct DivisionMetalShaderProgram {
    __strong id<MTLFunction> vertex_function;
    __strong id<MTLFunction> fragment_function;
} DivisionMetalShaderProgram;

typedef struct DivisionShaderSystemContext {
    DivisionMetalShaderProgram* shader_programs;
    int32_t shader_program_count;
} DivisionShaderSystemContext ;
