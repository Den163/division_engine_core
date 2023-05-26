#include "DivisionOSXViewDelegate.h"

#include "division_engine_core/renderer.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/uniform_buffer.h"
#include "osx_render_pass.h"
#include "osx_texture.h"
#include "osx_vertex_buffer.h"
#include "osx_uniform_buffer.h"

@implementation DivisionOSXViewDelegate
- (instancetype)initWithContext:(DivisionContext*)aContext
                       settings:(const DivisionSettings*)aSettings
                         device:(id)aDevice {
    self = [super init];
    if (self)
    {
        context = aContext;
        settings = aSettings;
        device = aDevice;
        commandQueue = [device newCommandQueue];
    }

    return self;
}

+ (instancetype)withContext:(DivisionContext*)aContext
                   settings:(const DivisionSettings*)aSettings
                     device:(id)aDevice {
    return [[self alloc] initWithContext:aContext settings:aSettings device:aDevice];
}

- (void)drawInMTKView:(nonnull MTKView*)view {
    settings->update_callback(context);

    @autoreleasepool
    {
        DivisionColor clearColor = context->renderer_context->clear_color;
        [view setClearColor:MTLClearColorMake(clearColor.r, clearColor.g, clearColor.b, clearColor.a)];

        id <MTLCommandBuffer> cmdBuffer = [commandQueue commandBuffer];
        MTLRenderPassDescriptor* renderPassDesc = [view currentRenderPassDescriptor];
        id <MTLRenderCommandEncoder> renderEnc = [cmdBuffer renderCommandEncoderWithDescriptor:renderPassDesc];

        DivisionRenderPassSystemContext* render_pass_ctx = context->render_pass_context;
        DivisionVertexBufferSystemContext* vert_buff_ctx = context->vertex_buffer_context;
        DivisionUniformBufferSystemContext* uniform_buff_ctx = context->uniform_buffer_context;
        DivisionTextureSystemContext* tex_ctx = context->texture_context;

        uint32_t* render_pass_ids = render_pass_ctx->id_table.orders;
        for (int32_t i = 0; i < render_pass_ctx->id_table.orders_count; i++)
        {
            const DivisionRenderPass* pass = &render_pass_ctx->render_passes[render_pass_ids[i]];
            const float* blend_color = pass->alpha_blending_options.constant_blend_color;
            const DivisionRenderPassInternalPlatform_* pass_impl = &render_pass_ctx->render_passes_impl[i];
            const DivisionVertexBufferInternalPlatform_* vert_buffer_impl =
                &vert_buff_ctx->buffers_impl[pass->vertex_buffer];

            id <MTLRenderPipelineState> pipelineState = pass_impl->mtl_pipeline_state;
            id <MTLBuffer> vertDataMtlBuffer = vert_buffer_impl->mtl_vertex_buffer;

            [renderEnc setRenderPipelineState:pipelineState];
            [renderEnc setVertexBuffer:vertDataMtlBuffer offset:0 atIndex:DIVISION_MTL_VERTEX_DATA_BUFFER_INDEX];

            for (int ubIdx = 0; ubIdx < pass->uniform_vertex_buffer_count; ubIdx++)
            {
                uint32_t buff_id = pass->uniform_vertex_buffers[ubIdx];
                id <MTLBuffer> uniformBuff = uniform_buff_ctx->uniform_buffers_impl[buff_id].mtl_buffer;
                DivisionUniformBufferDescriptor buffDesc = uniform_buff_ctx->uniform_buffers[buff_id];
                [renderEnc setVertexBuffer:uniformBuff offset:0 atIndex:buffDesc.binding];
            }

            for (int ubIdx = 0; ubIdx < pass->uniform_fragment_buffer_count; ubIdx++)
            {
                uint32_t buff_id = pass->uniform_fragment_buffers[ubIdx];
                id <MTLBuffer> uniformBuff = uniform_buff_ctx->uniform_buffers_impl[buff_id].mtl_buffer;
                DivisionUniformBufferDescriptor buffDesc = uniform_buff_ctx->uniform_buffers[buff_id];
                [renderEnc setFragmentBuffer:uniformBuff offset:0 atIndex:buffDesc.binding];
            }

            for (int texIdx = 0; texIdx < pass->fragment_texture_count; texIdx++)
            {
                const DivisionIdWithBinding* texture_binding = &pass->fragment_textures[texIdx];
                const DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[texture_binding->id];

                [renderEnc setFragmentTexture:tex_impl->mtl_texture atIndex:texture_binding->shader_location];
            }

            [renderEnc
                setBlendColorRed: blend_color[0] green: blend_color[1] blue: blend_color[2] alpha: blend_color[3]];

            if (pass->instance_count > 0)
            {
                [renderEnc setVertexBuffer:vertDataMtlBuffer
                                    offset:0
                                   atIndex:DIVISION_MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX];

                [renderEnc drawPrimitives:vert_buffer_impl->mtl_primitive_type
                              vertexStart:pass->first_vertex
                              vertexCount:pass->vertex_count
                            instanceCount:pass->instance_count
                ];
            }
            else
            {
                [renderEnc drawPrimitives:vert_buffer_impl->mtl_primitive_type
                              vertexStart:pass->first_vertex
                              vertexCount:pass->vertex_count
                ];
            }
        }

        [renderEnc endEncoding];
        [cmdBuffer presentDrawable:[view currentDrawable]];
        [cmdBuffer commit];
    }
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size {
}
@end
