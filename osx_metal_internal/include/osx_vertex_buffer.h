#pragma once

#include <MetalKit/MetalKit.hpp>

struct DivisionVertexBufferInternalPlatform_
{
    MTL::Buffer* mtl_buffer;
    MTL::VertexDescriptor* mtl_vertex_descriptor;
};