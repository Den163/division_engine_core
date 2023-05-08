#pragma once

#include <Metal/Metal.h>
#include <MetalKit/MetalKit.h>

typedef struct DivisionShaderInternal_ {
    __strong id<MTLFunction> vertex_function;
    __strong id<MTLFunction> fragment_function;
} DivisionMetalShaderProgram;
