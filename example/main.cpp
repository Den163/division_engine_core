#include <memory.h>
#include <cstdio>

#include "division_engine_core/render_pass.h"
#include "division_engine_core/renderer.h"
#include "division_engine_core/shader.h"
#include "division_engine_core/uniform_buffer.h"
#include "division_engine_core/vertex_buffer.h"
#include "division_engine_core/io_utility.h"
#include "division_engine_shader_compiler/interface.h"

void error_callback(int error_code, const char* message);
void init_callback(DivisionContext* ctx);
void update_callback(DivisionContext* ctx);

typedef struct VertexData
{
    float position[3];
    float color[4];
} VertexData;

int main()
{
    char* shader_data = nullptr;
    void* spv_data = nullptr;
    size_t shader_size = 0;
    size_t spv_size = 0;

    division_io_read_all_bytes_from_file("test.vert", (void**) &shader_data, &shader_size);
    division_engine_shader_compiler_alloc();

    bool ret = division_engine_shader_compiler_compile_glsl_to_spirv(
        shader_data, DIVISION_SHADER_VERTEX, "vert", &spv_data, &spv_size);
    if (ret)
    {
        printf("Compiled glsl to spirv. Total size: %zu\n", spv_size);
    }

    division_engine_shader_compiler_free();

    free(shader_data);
    free(spv_data);

    DivisionSettings settings = {
        .window_width = 512,
        .window_height = 512,
        .window_title = "New window",
        .error_callback = error_callback,
        .init_callback = init_callback,
        .update_callback = update_callback,
    };

    DivisionContext* ctx = nullptr;
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
            .file_path = "test.metal",
            .entry_point_name = "vertexMain",
        },
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_FRAGMENT,
            .file_path = "test.metal",
            .entry_point_name = "fragmentMain",
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

    uint32_t shader_program;
    division_engine_shader_program_alloc(ctx, shader_settings, source_count, &shader_program);

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

    uint32_t vertex_buffer;
    division_engine_vertex_buffer_alloc(
        ctx, attr, attr_count, vertex_count, DIVISION_TOPOLOGY_TRIANGLES, &vertex_buffer);
    auto* vert_buff_ptr = static_cast<VertexData*>(
        division_engine_vertex_buffer_borrow_data_pointer(ctx, vertex_buffer));
    memcpy(vert_buff_ptr, vd, sizeof(vd));
    division_engine_vertex_buffer_return_data_pointer(ctx, vertex_buffer, vert_buff_ptr);

    float testVec[] = { 0, 1, 0, 1 };

    DivisionUniformBuffer buff = {
        .data_bytes = sizeof(testVec),
        .binding = 1,
        .shaderType = DIVISION_SHADER_FRAGMENT
    };

    uint32_t uniform_buffer;
    division_engine_uniform_buffer_alloc(ctx, buff, &uniform_buffer);

    auto* uniform_ptr = static_cast<float*>(division_engine_uniform_buffer_borrow_data_pointer(ctx, uniform_buffer));
    memcpy(uniform_ptr, testVec, sizeof(testVec));
    division_engine_uniform_buffer_return_data_pointer(ctx, uniform_buffer, uniform_ptr);

    uint32_t render_pass_id;
    division_engine_render_pass_alloc(ctx, (DivisionRenderPass) {
        .first_vertex = 0,
        .vertex_count = static_cast<size_t>(vertex_count),
        .uniform_buffers = &uniform_buffer,
        .uniform_buffer_count = 1,
        .vertex_buffer = vertex_buffer,
        .shader_program = shader_program,
    }, &render_pass_id);
}

void update_callback(DivisionContext* ctx)
{
}

void error_callback(int error_code, const char* message)
{
    fprintf(stderr, "Error code: %d, error message: %s\n", error_code, message);
}