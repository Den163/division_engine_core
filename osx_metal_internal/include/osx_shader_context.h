#pragma once

#include <MetalKit/MetalKit.hpp>
#include <unordered_map>
#include <string>

typedef struct DivisionMetalAttribute DivisionMetalAttribute;

typedef struct DivisionMetalShaderProgram {
    MTL::Function* vertex_function;
    MTL::Function* fragment_function;
} DivisionMetalShaderProgram;

typedef struct DivisionShaderSystemContext {
    DivisionMetalShaderProgram* shader_programs;
    size_t shader_program_count;
} DivisionShaderSystemContext ;
