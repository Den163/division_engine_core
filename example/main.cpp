#include <memory.h>
#include <cstdio>

#include <division_engine_core/render_pass.h>
#include <division_engine_core/renderer.h>
#include <division_engine_core/shader.h>
#include <division_engine_core/texture.h>
#include <division_engine_core/uniform_buffer.h>
#include <division_engine_core/vertex_buffer.h>
#include <division_engine_core/io_utility.h>
#include <division_shader_compiler/interface.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

static void compile_to_metal_shaders();
static void error_callback(int error_code, const char* message);
static void init_callback(DivisionContext* ctx);
static void update_callback(DivisionContext* ctx);

typedef struct VertexData
{
    float position[3];
    float color[4];
    float uv[2];
} VertexData;

int main()
{
#ifdef __APPLE__
    compile_to_metal_shaders();
#endif

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

void compile_to_metal_shaders()
{
    division_shader_compiler_alloc();

    const size_t shader_count = 2;
    DivisionShaderSettings shader_to_compile[shader_count] = {
        {.type = DIVISION_SHADER_VERTEX, .file_path = "test.vert", .entry_point_name = "vert",},
        {.type = DIVISION_SHADER_FRAGMENT, .file_path = "test.frag", .entry_point_name = "frag"}
    };
    const char* output_names[shader_count] = {"./test.vert.metal", "./test.frag.metal"};

    for (int i = 0; i < shader_count; i++)
    {
        char* shader_data = nullptr;
        void* spv_data = nullptr;
        char* msl_data = nullptr;

        size_t shader_size = 0;
        size_t spv_size = 0;
        size_t msl_size = 0;

        DivisionShaderSettings shader_setting = shader_to_compile[i];
        division_io_read_all_bytes_from_file(shader_setting.file_path, (void**) &shader_data, &shader_size);

        division_shader_compiler_compile_glsl_to_spirv(
            shader_data,
            (int32_t) shader_size,
            shader_setting.type,
            shader_setting.entry_point_name,
            &spv_data,
            &spv_size);
        division_shader_compiler_compile_spirv_to_metal(
            spv_data, spv_size, shader_setting.type, shader_setting.entry_point_name, &msl_data, &msl_size);

        division_io_write_all_bytes_to_file(output_names[i], msl_data, msl_size);

        free(shader_data);
        free(spv_data);
        free(msl_data);
    }

    division_shader_compiler_free();
}

void init_callback(DivisionContext* ctx)
{
#if __APPLE__
    DivisionShaderSettings shader_settings[] = {
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_VERTEX,
            .file_path = "test.vert.metal",
            .entry_point_name = "vert",
        },
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_FRAGMENT,
            .file_path = "test.frag.metal",
            .entry_point_name = "frag",
        }
    };
#else
    DivisionShaderSettings shader_settings[] = {
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_VERTEX,
            .file_path = "test.vert",
            .entry_point_name = "main",
        },
        (DivisionShaderSettings) {
            .type = DIVISION_SHADER_FRAGMENT,
            .file_path = "test.frag",
            .entry_point_name = "main",
        }
    };
#endif
    int32_t source_count = sizeof(shader_settings) / sizeof(DivisionShaderSettings);

    uint32_t shader_program;
    division_engine_shader_program_alloc(ctx, shader_settings, source_count, &shader_program);

    VertexData vd[] = {
        {.position = {-0.5f, -0.5f, 0}, .color = {1, 1, 1, 1}, .uv = {0, 1}},
        {.position = {0, 0, 0}, .color = {1, 1, 1, 1}, .uv = {1, 0}},
        {.position = {-0.5f, 0, 0}, .color = {1, 1, 1, 1}, .uv = {0, 0}},
        {.position = {0, 0, 0}, .color = {1, 1, 1, 1}, .uv = {1, 0}},
        {.position = {-0.5f, -0.5f, 0}, .color = {1, 1, 1, 1}, .uv = {0, 1}},
        {.position = {0, -0.5f, 0}, .color = {1, 1, 1, 1}, .uv = {1, 1}},
    };
    float local_to_word_mat[] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1, // 1
        1, 0, 0, 1,
        0, 1, 0, 1,
        0, 0, 1, 0,
        0, 0, 0, 1 // 2
    };
    int32_t vertex_count = sizeof(vd) / sizeof(VertexData);
    int32_t instance_count = sizeof(local_to_word_mat) / (sizeof(float) * 16);

    DivisionVertexAttributeSettings vertex_attrs[] = {
        {.type = DIVISION_FVEC3, .location = 0},
        {.type = DIVISION_FVEC4, .location = 1},
        {.type = DIVISION_FVEC2, .location = 2},
    };
    DivisionVertexAttributeSettings instance_attrs[] = {{.type = DIVISION_FMAT4X4, .location = 3}};

    DivisionVertexBufferSettings vertex_buffer_settings = {
        .per_vertex_attributes = vertex_attrs,
        .per_instance_attributes = instance_attrs,
        .per_vertex_attribute_count = 3,
        .per_instance_attribute_count = 1,
        .vertex_count = vertex_count,
        .instance_count = instance_count,
        .topology = DIVISION_TOPOLOGY_TRIANGLES,
    };

    uint32_t vertex_buffer;
    division_engine_vertex_buffer_alloc(ctx, &vertex_buffer_settings, &vertex_buffer);

    auto* vert_buff_ptr = static_cast<int8_t*>(
        division_engine_vertex_buffer_borrow_data_pointer(ctx, vertex_buffer));

    memcpy(vert_buff_ptr, vd, sizeof(vd));

    auto* per_inst_ptr = vert_buff_ptr + sizeof(vd);
    memcpy(per_inst_ptr, local_to_word_mat, sizeof(local_to_word_mat));

    division_engine_vertex_buffer_return_data_pointer(ctx, vertex_buffer, vert_buff_ptr);

    float testVec[] = {0, 0.5f, 0, 1};

    DivisionUniformBufferDescriptor buff = {.data_bytes = sizeof(testVec), .binding = 1,};

    uint32_t uniform_buffer;
    division_engine_uniform_buffer_alloc(ctx, buff, &uniform_buffer);

    auto* uniform_ptr = static_cast<float*>(division_engine_uniform_buffer_borrow_data_pointer(ctx, uniform_buffer));
    memcpy(uniform_ptr, testVec, sizeof(testVec));
    division_engine_uniform_buffer_return_data_pointer(ctx, uniform_buffer, uniform_ptr);

    int image_width, image_height, channels_in_file;
    void* tex_data = stbi_load("nevsky.jpg", &image_width, &image_height, &channels_in_file, 3);

    DivisionTexture texture = {
        .texture_format = DIVISION_TEXTURE_FORMAT_RGB24Uint, .width = (uint32_t) image_width, .height = (uint32_t)
            image_height
    };
    uint32_t tex_id;
    division_engine_texture_alloc(ctx, &texture, &tex_id);
    division_engine_texture_set_data(ctx, tex_id, tex_data);
    stbi_image_free(tex_data);

    DivisionIdWithBinding frag_textures[] = { DivisionIdWithBinding { .id = tex_id, .shader_location = 0 } };

    uint32_t render_pass_id;
    division_engine_render_pass_alloc(ctx, (DivisionRenderPass) {
        .first_vertex = 0,
        .vertex_count = static_cast<size_t>(vertex_count),
        .instance_count = static_cast<size_t>(instance_count),
        .uniform_vertex_buffer_count = 0,
        .uniform_fragment_buffers = &uniform_buffer,
        .uniform_fragment_buffer_count = 1,
        .fragment_textures = frag_textures,
        .fragment_texture_count = 1,
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