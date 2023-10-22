#include "division_engine_core/platform_internal/platform_render_pass_descriptor.h"
#include "division_engine_core/platform_internal/platform_render_pass_instance.h"

#include "division_engine_core/render_pass_descriptor.h"
#include "division_engine_core/texture.h"
#include "division_engine_core/utility.h"
#include "division_engine_core/vertex_buffer.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/renderer.h"

#include "glfw_render_pass.h"
#include "glfw_shader.h"
#include "glfw_texture.h"
#include "glfw_uniform_buffer.h"
#include "glfw_vertex_buffer.h"

static inline void bind_uniform_buffer(
    DivisionUniformBufferSystemContext* ctx, const DivisionIdWithBinding* buffer_binding
);

void division_engine_internal_platform_render_pass_instance_draw(
    DivisionContext* ctx,
    const DivisionRenderPassInstance* render_pass_instances,
    uint32_t render_pass_instance_count
)
{
    DivisionRendererSystemContext* renderer_ctx = ctx->renderer_context;
    DivisionVertexBufferSystemContext* vert_buff_ctx = ctx->vertex_buffer_context;
    DivisionUniformBufferSystemContext* uniform_buff_ctx = ctx->uniform_buffer_context;
    DivisionRenderPassSystemContext* render_pass_ctx = ctx->render_pass_context;
    DivisionShaderSystemContext* shader_ctx = ctx->shader_context;
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;

    glClearBufferfv(GL_COLOR, 0, (const GLfloat*)&renderer_ctx->clear_color);

    for (int32_t i = 0; i < render_pass_instance_count; i++)
    {
        const DivisionRenderPassInstance* pass_instance = &render_pass_instances[i];
        uint32_t pass_desc_id = pass_instance->render_pass_descriptor_id;
        const DivisionRenderPassDescriptor* pass_desc =
            &render_pass_ctx->render_pass_descriptors[pass_desc_id];
        DivisionRenderPassInternalPlatform_* pass_desc_impl =
            &render_pass_ctx->render_passes_descriptors_impl[pass_desc_id];
        DivisionVertexBufferInternalPlatform_ vb_internal =
            vert_buff_ctx->buffers_impl[pass_desc->vertex_buffer_id];
        DivisionShaderInternal_ shader_internal =
            shader_ctx->shaders_impl[pass_desc->shader_program];
        const float* const_blend_color =
            pass_desc->alpha_blending_options.constant_blend_color;

        glBindVertexArray(vb_internal.gl_vao);
        glBindBuffer(GL_ARRAY_BUFFER, vb_internal.gl_vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vb_internal.gl_index_buffer);
        glUseProgram(shader_internal.gl_shader_program);

        for (int uniform_idx = 0;
             uniform_idx < pass_instance->uniform_vertex_buffer_count;
             uniform_idx++)
        {
            bind_uniform_buffer(
                uniform_buff_ctx, &pass_instance->uniform_vertex_buffers[uniform_idx]
            );
        }

        for (int uniform_idx = 0;
             uniform_idx < pass_instance->uniform_fragment_buffer_count;
             uniform_idx++)
        {
            bind_uniform_buffer(
                uniform_buff_ctx, &pass_instance->uniform_fragment_buffers[uniform_idx]
            );
        }

        for (int frag_tex_idx = 0; frag_tex_idx < pass_instance->fragment_texture_count;
             frag_tex_idx++)
        {
            DivisionIdWithBinding tex_bind =
                pass_instance->fragment_textures[frag_tex_idx];
            DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[tex_bind.id];

            glBindTextureUnit(tex_bind.shader_location, tex_impl->gl_texture);
        }

        if (division_mask_has_flag(
                pass_desc->capabilities_mask,
                DIVISION_RENDER_PASS_DESCRIPTOR_CAPABILITY_ALPHA_BLEND
            ))
        {
            glEnable(GL_BLEND);

            glBlendFunc(pass_desc_impl->gl_blend_src, pass_desc_impl->gl_blend_dst);
            glBlendEquation(pass_desc_impl->gl_blend_equation);
            glBlendColor(
                const_blend_color[0],
                const_blend_color[1],
                const_blend_color[2],
                const_blend_color[3]
            );
        }
        else
        {
            glDisable(GL_BLEND);
        }

        glColorMask(
            division_mask_has_flag(pass_desc->color_mask, DIVISION_COLOR_MASK_R),
            division_mask_has_flag(pass_desc->color_mask, DIVISION_COLOR_MASK_G),
            division_mask_has_flag(pass_desc->color_mask, DIVISION_COLOR_MASK_B),
            division_mask_has_flag(pass_desc->color_mask, DIVISION_COLOR_MASK_A)
        );

        if (division_mask_has_flag(
                pass_desc->capabilities_mask,
                DIVISION_RENDER_PASS_INSTANCE_CAPABILITY_INSTANCED_RENDERING
            ))
        {
            glDrawElementsInstancedBaseVertex(
                vb_internal.gl_topology,
                pass_instance->index_count,
                GL_UNSIGNED_INT,
                NULL,
                pass_instance->instance_count,
                pass_instance->first_vertex
            );
        }
        else
        {
            glDrawElementsBaseVertex(
                vb_internal.gl_topology,
                pass_instance->index_count,
                GL_UNSIGNED_INT,
                NULL,
                pass_instance->first_vertex
            );
        }
    }
}

void bind_uniform_buffer(
    DivisionUniformBufferSystemContext* ctx, const DivisionIdWithBinding* buffer_binding
)
{
    uint32_t buffer_id = buffer_binding->id;
    GLuint gl_uniform_buff = ctx->uniform_buffers_impl[buffer_id].gl_buffer;

    glBindBufferBase(GL_UNIFORM_BUFFER, buffer_binding->shader_location, gl_uniform_buff);
}