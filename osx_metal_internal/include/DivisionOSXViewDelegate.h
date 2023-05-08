#pragma once

#include <Foundation/Foundation.h>
#include <MetalKit/MetalKit.h>
#include <Metal/Metal.h>

#include "division_engine/context.h"
#include "division_engine/vertex_buffer.h"
#include "osx_shader_context.h"

@interface DivisionOSXViewDelegate : NSObject <MTKViewDelegate>
{
@public
    DivisionContext* context;
@public
    const DivisionSettings* settings;
@public
    id <MTLDevice> device;
@public
    id <MTLCommandQueue> commandQueue;
};

+ (instancetype)withContext:(DivisionContext*)aContext
                   settings:(const DivisionSettings*)aSettings
                     device:(id)aDevice;

- (instancetype)initWithContext:(DivisionContext*)aContext
                       settings:(const DivisionSettings*)aSettings
                         device:(id)aDevice;

- (id <MTLBuffer>)createBufferWithSize:(size_t)data_bytes;

- (MTLVertexDescriptor*)createVertexDescriptorForBuffer:(const DivisionVertexBuffer*)vertexBuffer;


- (id <MTLRenderPipelineState>)createRenderPipelineStateForShaderProgram:(const DivisionMetalShaderProgram*)program
                                                        vertexDescriptor:(MTLVertexDescriptor*)desc;

- (bool)createShaderProgramWithSettings:
    (const DivisionShaderSettings*)shaderSettings
                            sourceCount:(int32_t)sourceCount
                             outProgram:(DivisionMetalShaderProgram*)out_program;

@end