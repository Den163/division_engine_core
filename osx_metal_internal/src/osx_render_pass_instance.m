#include "division_engine_core/render_pass_instance.h"
#include "division_engine_core/utility.h"
#include "osx_render_pass.h"
#include "osx_texture.h"
#include "osx_uniform_buffer.h"
#include "osx_vertex_buffer.h"
#include "osx_window_context.h"
#include <MetalKit/MetalKit.h>

#include <division_engine_core/platform_internal/platform_render_pass_instance.h>
#include <division_engine_core/renderer.h>
#include <division_engine_core/texture.h>
#include <division_engine_core/uniform_buffer.h>

#include <division_engine_core/context.h>

void division_engine_internal_platform_render_pass_instance_draw(
    DivisionContext* context,
    const DivisionRenderPassInstance* render_pass_instances,
    uint32_t render_pass_instance_count
)
{
    DivisionOSXWindowContext* window_context = context->renderer_context->window_data;
    MTKView* view = window_context->app_delegate->view;
    id<MTLCommandQueue> commandQueue =
        window_context->app_delegate->viewDelegate->commandQueue;

    @autoreleasepool
    {
        const DivisionColor clearColor = context->renderer_context->clear_color;
        [view setClearColor:MTLClearColorMake(
                                clearColor.r, clearColor.g, clearColor.b, clearColor.a
                            )];

        id<MTLCommandBuffer> cmdBuffer = [commandQueue commandBuffer];
        MTLRenderPassDescriptor* renderPassDesc = [view currentRenderPassDescriptor];
        id<MTLRenderCommandEncoder> renderEnc =
            [cmdBuffer renderCommandEncoderWithDescriptor:renderPassDesc];

        const DivisionRenderPassSystemContext* render_pass_ctx =
            context->render_pass_context;
        const DivisionVertexBufferSystemContext* vert_buff_ctx =
            context->vertex_buffer_context;
        const DivisionUniformBufferSystemContext* uniform_buff_ctx =
            context->uniform_buffer_context;
        const DivisionTextureSystemContext* tex_ctx = context->texture_context;

        const CGSize drawable_size = [view drawableSize];
        [renderEnc setViewport:(MTLViewport){
                                   .originX = 0,
                                   .originY = 0,
                                   .width = drawable_size.width,
                                   .height = drawable_size.height,
                                   .znear = -1.0f,
                                   .zfar = 1.0f,
                               }];
        [renderEnc setFrontFacingWinding:MTLWindingCounterClockwise];

        for (size_t i = 0; i < render_pass_instance_count; i++)
        {
            const DivisionRenderPassInstance* render_pass_instance =
                &render_pass_instances[i];

            const uint32_t render_pass_descriptor_id =
                render_pass_instances->render_pass_descriptor_id;
            const DivisionRenderPassDescriptor* pass_desc =
                &render_pass_ctx->render_pass_descriptors[render_pass_descriptor_id];
            const DivisionRenderPassInternalPlatform_* pass_desc_impl =
                &render_pass_ctx
                     ->render_passes_descriptors_impl[render_pass_descriptor_id];
            const DivisionVertexBufferInternalPlatform_* vert_buffer_impl =
                &vert_buff_ctx->buffers_impl[pass_desc->vertex_buffer_id];

            id<MTLRenderPipelineState> pipelineState = pass_desc_impl->mtl_pipeline_state;
            id<MTLBuffer> vertDataMtlBuffer = vert_buffer_impl->mtl_vertex_buffer;

            [renderEnc setRenderPipelineState:pipelineState];
            [renderEnc setVertexBuffer:vertDataMtlBuffer
                                offset:0
                               atIndex:DIVISION_MTL_VERTEX_DATA_BUFFER_INDEX];

            const float* blend_color =
                pass_desc->alpha_blending_options.constant_blend_color;
            [renderEnc setBlendColorRed:blend_color[0]
                                  green:blend_color[1]
                                   blue:blend_color[2]
                                  alpha:blend_color[3]];

            size_t vert_uniform_count = render_pass_instance->uniform_vertex_buffer_count;
            for (int ubIdx = 0; ubIdx < vert_uniform_count; ubIdx++)
            {
                const DivisionIdWithBinding* buff_binding =
                    &render_pass_instance->uniform_vertex_buffers[ubIdx];
                id<MTLBuffer> uniformBuff =
                    uniform_buff_ctx->uniform_buffers_impl[buff_binding->id].mtl_buffer;
                [renderEnc setVertexBuffer:uniformBuff
                                    offset:0
                                   atIndex:buff_binding->shader_location];
            }

            size_t frag_uniform_count =
                render_pass_instance->uniform_fragment_buffer_count;
            for (int ubIdx = 0; ubIdx < frag_uniform_count; ubIdx++)
            {
                const DivisionIdWithBinding* buff_binding =
                    &render_pass_instance->uniform_fragment_buffers[ubIdx];
                id<MTLBuffer> uniformBuff =
                    uniform_buff_ctx->uniform_buffers_impl[buff_binding->id].mtl_buffer;
                [renderEnc setFragmentBuffer:uniformBuff
                                      offset:0
                                     atIndex:buff_binding->shader_location];
            }

            size_t texture_count = render_pass_instance->fragment_texture_count;
            for (int texIdx = 0; texIdx < texture_count; texIdx++)
            {
                const DivisionIdWithBinding* texture_binding =
                    &render_pass_instance->fragment_textures[texIdx];
                const DivisionTextureImpl_* tex_impl =
                    &tex_ctx->textures_impl[texture_binding->id];

                [renderEnc setFragmentTexture:tex_impl->mtl_texture
                                      atIndex:texture_binding->shader_location];
                [renderEnc setFragmentSamplerState:tex_impl->mtl_sampler
                                           atIndex:texture_binding->shader_location];
            }

            if (division_mask_has_flag(
                    render_pass_instance->capabilities_mask,
                    DIVISION_RENDER_PASS_INSTANCE_CAPABILITY_INSTANCED_RENDERING
                ))
            {
                [renderEnc setVertexBuffer:vertDataMtlBuffer
                                    offset:0
                                   atIndex:DIVISION_MTL_VERTEX_DATA_INSTANCE_ARRAY_INDEX];

                [renderEnc drawIndexedPrimitives:vert_buffer_impl->mtl_primitive_type
                                      indexCount:render_pass_instance->index_count
                                       indexType:MTLIndexTypeUInt32
                                     indexBuffer:vert_buffer_impl->mtl_index_buffer
                               indexBufferOffset:0
                                   instanceCount:render_pass_instance->instance_count
                                      baseVertex:render_pass_instance->first_vertex
                                    baseInstance:render_pass_instance->first_instance];
            }
            else
            {
                [renderEnc drawIndexedPrimitives:vert_buffer_impl->mtl_primitive_type
                                      indexCount:render_pass_instance->index_count
                                       indexType:MTLIndexTypeUInt32
                                     indexBuffer:vert_buffer_impl->mtl_index_buffer
                               indexBufferOffset:0
                                   instanceCount:1
                                      baseVertex:0
                                    baseInstance:0];
            }
        }

        [renderEnc endEncoding];
        [cmdBuffer presentDrawable:[view currentDrawable]];
        [cmdBuffer commit];
    }
}