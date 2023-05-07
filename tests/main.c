#include <stdio.h>
#include <memory.h>
#include <assert.h>

#include "division_engine/render_pass.h"
#include "division_engine/renderer.h"
#include "division_engine/shader.h"
#include "division_engine/uniform_buffer.h"
#include "division_engine/vertex_buffer.h"

void error_callback(int error_code, const char* message);
void init_callback(DivisionContext* ctx);
void update_callback(DivisionContext* ctx);

typedef struct UserData
{
    int32_t shader_id;
} UserData;

typedef struct VertexData
{
    float position[3];
    float color[4];
} VertexData;

int main()
{
    DivisionSettings settings = {
        .window_title = "New window",
        .window_width = 512,
        .window_height = 512,
        .init_callback = init_callback,
        .update_callback = update_callback,
        .error_callback = error_callback,
    };

    DivisionContext* ctx = NULL;
    division_engine_context_alloc(&settings, &ctx);

    division_engine_renderer_run_loop(ctx, &settings);
    division_engine_context_free(ctx);
}

void init_callback(DivisionContext* ctx)
{
#if __APPLE__
    DivisionShaderSettings shader_settings[] = {
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_VERTEX,
            .entry_point_name = "vertexMain",
            .file_path = "test.metal"
        },
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_FRAGMENT,
            .entry_point_name = "fragmentMain",
            .file_path = "test.metal"
        }
    };
#else
    DivisionShaderSettings shader_settings[] = {
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_VERTEX,
            .entry_point_name = "main",
            .file_path = "test.vert"
        },
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_FRAGMENT,
            .entry_point_name = "main",
            .file_path = "test.frag"
        }
    };
#endif
    int32_t source_count = sizeof(shader_settings) / sizeof(DivisionShaderSettings);

    int32_t shader_program = division_engine_shader_program_create(ctx, shader_settings, source_count);
    assert(shader_program >= 0);

    VertexData vd[] = {
        { .position = {-0.5f, -0.5f, 0}, .color = {1, 1, 1, 1} },
        { .position = {0, 0, 0}, .color = {1, 1, 1, 1} },
        { .position = {-0.5f, 0, 0}, .color = {1, 1, 1, 1} },
        { .position = {0, 0, 0}, .color = {1, 1, 1, 1} },
        { .position = {-0.5f, -0.5f, 0}, .color = {1, 1, 1, 1} },
        { .position = {0, -0.5f, 0}, .color = {1, 1, 1, 1} },
    };
    int32_t vertex_count = sizeof(vd) / sizeof(VertexData);

    DivisionVertexAttributeSettings attr[] = {
        {.type = DIVISION_FVEC3, .location = 0},
        {.type = DIVISION_FVEC4, .location = 1}
    };

    int32_t attr_count = sizeof(attr) / sizeof(DivisionVertexAttributeSettings);

    int32_t vertex_buffer = division_engine_vertex_buffer_alloc(
        ctx, attr, attr_count, vertex_count, DIVISION_TOPOLOGY_TRIANGLES);
    VertexData* vert_buff_ptr = division_engine_vertex_buffer_borrow_data_pointer(ctx, vertex_buffer);
    memcpy(vert_buff_ptr, vd, sizeof(vd));
    division_engine_vertex_buffer_return_data_pointer(ctx, vertex_buffer, vert_buff_ptr);

    float testVec[] = { 0, 1, 0, 1 };

    DivisionUniformBuffer buff = {
        .data_bytes = sizeof(testVec),
        .binding = 1,
        .shaderType = DIVISION_SHADER_FRAGMENT
    };
    int32_t uniform_buffer = division_engine_uniform_buffer_alloc(ctx, buff);
    float* uniform_ptr = division_engine_uniform_buffer_borrow_data_pointer(ctx, uniform_buffer);
    memcpy(uniform_ptr, testVec, sizeof(testVec));
    division_engine_uniform_buffer_return_data_pointer(ctx, uniform_buffer, uniform_ptr);

    division_engine_render_pass_alloc(ctx, (DivisionRenderPass) {
        .vertex_buffer = vertex_buffer,
        .shader_program = shader_program,
        .uniform_buffers = &uniform_buffer,
        .uniform_buffer_count = 1,
        .first_vertex = 0,
        .vertex_count = vertex_count
    });
}

void update_callback(DivisionContext* ctx)
{
}

void error_callback(int error_code, const char* message)
{
    fprintf(stderr, "Error code: %d, error message: %s\n", error_code, message);
}