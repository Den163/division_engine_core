#pragma once

#include <MetalKit/MetalKit.h>

typedef struct DivisionVertexBufferInternalPlatform_
{
    __strong id<MTLBuffer> mtl_vertex_buffer;
    __strong MTLVertexDescriptor* mtl_vertex_descriptor;
    MTLPrimitiveType mtl_primitive_type;

} DivisionVertexBufferInternalPlatform_;