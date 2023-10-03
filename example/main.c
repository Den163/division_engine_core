#include "division_engine_core/context.h"
#include "division_engine_core/division_lifecycle.h"
#include "division_engine_core/font.h"
#include <division_engine_core/io_utility.h>
#include <division_engine_core/render_pass.h>
#include <division_engine_core/renderer.h>
#include <division_engine_core/shader.h>
#include <division_engine_core/texture.h>
#include <division_engine_core/uniform_buffer.h>
#include <division_engine_core/vertex_buffer.h>

#include <assert.h>
#include <memory.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>

static void error_callback(DivisionContext* ctx, int error_code, const char* message);
static void init_callback(DivisionContext* ctx);
static void update_callback(DivisionContext* ctx);

static void example_create_shader_program(DivisionContext* ctx, uint32_t* out_shader_id);

static void example_create_vertex_buffer(
    DivisionContext* ctx,
    uint32_t* out_vertex_buffer_id,
    size_t* out_vertex_count,
    size_t* out_index_count,
    size_t* out_instance_count
);

static void example_create_uniform_buffer(
    DivisionContext* ctx, DivisionIdWithBinding* out_uniform_buffer
);

static void example_create_textures(
    DivisionContext* ctx, DivisionIdWithBinding* out_texture
);

static void example_write_font_character(DivisionContext* ctx);

typedef struct VertexData
{
    float position[3];
    float color[4];
    float uv[2];
} VertexData;

int main()
{
    DivisionSettings settings = {
        .window_width = 512,
        .window_height = 512,
        .window_title = "New window",
    };

    DivisionLifecycle lifecycle = {
        .init_callback = init_callback,
        .update_callback = update_callback,
        .free_callback = division_engine_context_finalize,
        .error_callback = error_callback,
    };

    DivisionContext ctx;
    assert(division_engine_context_initialize(&settings, &ctx));
    division_engine_context_register_lifecycle(&ctx, &lifecycle);

    division_engine_renderer_run_loop(&ctx);
}

void init_callback(DivisionContext* ctx)
{
    DivisionIdWithBinding uniform_buffer, texture;
    uint32_t vertex_buffer, shader_program;
    size_t vertex_count, index_count, instance_count;

    example_create_shader_program(ctx, &shader_program);
    example_create_uniform_buffer(ctx, &uniform_buffer);
    example_create_vertex_buffer(
        ctx, &vertex_buffer, &vertex_count, &index_count, &instance_count
    );

    example_write_font_character(ctx);
    example_create_textures(ctx, &texture);

    uint32_t render_pass_id;
    division_engine_render_pass_alloc(
        ctx,
        (DivisionRenderPass){
            .alpha_blending_options =
                (DivisionAlphaBlendingOptions){
                    .src = DIVISION_ALPHA_BLEND_SRC_ALPHA,
                    .dst = DIVISION_ALPHA_BLEND_ONE_MINUS_SRC_ALPHA,
                    .operation = DIVISION_ALPHA_BLEND_OP_ADD,
                    .constant_blend_color = {0, 0, 0, 0},
                },
            .first_vertex = 0,
            .vertex_count = (size_t)vertex_count,
            .instance_count = (size_t)instance_count,
            .index_count = (size_t)index_count,
            .uniform_vertex_buffer_count = 0,
            .uniform_fragment_buffers = &uniform_buffer,
            .uniform_fragment_buffer_count = 1,
            .fragment_textures = &texture,
            .fragment_texture_count = 1,
            .vertex_buffer = vertex_buffer,
            .shader_program = shader_program,
            .capabilities_mask = DIVISION_RENDER_PASS_CAPABILITY_ALPHA_BLEND |
                                 DIVISION_RENDER_PASS_CAPABILITY_INSTANCED_RENDERING,
            .color_mask = DIVISION_COLOR_MASK_RGBA,
        },
        &render_pass_id
    );
}

void update_callback(DivisionContext* ctx)
{
}

void error_callback(DivisionContext* ctx, int error_code, const char* message)
{
    fprintf(stderr, "Error code: %d, error message: %s\n", error_code, message);
}

static void example_create_shader_program(DivisionContext* ctx, uint32_t* out_shader_id)
{
    const char* vert_shader_entry_point;
    const char* frag_shader_entry_point;
    char* vert_shader_src;
    char* frag_shader_src;
    size_t vert_shader_size;
    size_t frag_shader_size;

#if __APPLE__
    vert_shader_entry_point = "vert";
    frag_shader_entry_point = "frag";

    division_io_read_all_bytes_from_file(
        "test.vert.metal", (void**)&vert_shader_src, &vert_shader_size
    );
    division_io_read_all_bytes_from_file(
        "test.frag.metal", (void**)&frag_shader_src, &frag_shader_size
    );
#else
    vert_shader_entry_point = "main";
    frag_shader_entry_point = "main";

    division_io_read_all_bytes_from_file(
        "test.vert", (void**)&vert_shader_src, &vert_shader_size
    );
    division_io_read_all_bytes_from_file(
        "test.frag", (void**)&frag_shader_src, &frag_shader_size
    );
#endif

    DivisionShaderSourceDescriptor shader_settings[] = {
        (DivisionShaderSourceDescriptor){
            .type = DIVISION_SHADER_VERTEX,
            .entry_point_name = vert_shader_entry_point,
            .source = vert_shader_src,
            .source_size = vert_shader_size,
        },
        (DivisionShaderSourceDescriptor){
            .type = DIVISION_SHADER_FRAGMENT,
            .entry_point_name = frag_shader_entry_point,
            .source = frag_shader_src,
            .source_size = frag_shader_size,
        }};

    int32_t source_count =
        sizeof(shader_settings) / sizeof(DivisionShaderSourceDescriptor);

    uint32_t shader_program;
    assert(division_engine_shader_program_alloc(
        ctx, shader_settings, source_count, &shader_program
    ));

    *out_shader_id = shader_program;
}

void example_create_vertex_buffer(
    DivisionContext* ctx,
    uint32_t* out_vertex_buffer_id,
    size_t* out_vertex_count,
    size_t* out_index_count,
    size_t* out_instance_count
)
{
    VertexData vd[] = {
        {.position = {-0.5f, 0, 0.5}, .color = {1, 1, 1, 1}, .uv = {0, 0}},
        {.position = {-0.5f, -0.5f, 0.5}, .color = {1, 1, 1, 1}, .uv = {0, 1}},
        {.position = {0, -0.5f, 0.5}, .color = {1, 1, 1, 1}, .uv = {1, 1}},
        {.position = {0, 0, 0.5}, .color = {1, 1, 1, 1}, .uv = {1, 0}},
    };

    uint32_t indices[] = {0, 1, 2, 2, 3, 0};

    float local_to_word_mat[] = {
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, // 1
        1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1  // 2
    };

    size_t vertex_count = sizeof(vd) / sizeof(VertexData);
    size_t index_count = sizeof(indices) / sizeof(int32_t);
    size_t instance_count = sizeof(local_to_word_mat) / (sizeof(float) * 16);

    DivisionVertexAttributeSettings vertex_attrs[] = {
        {.type = DIVISION_FVEC3, .location = 0},
        {.type = DIVISION_FVEC4, .location = 1},
        {.type = DIVISION_FVEC2, .location = 2},
    };
    DivisionVertexAttributeSettings instance_attrs[] = {
        {.type = DIVISION_FMAT4X4, .location = 3}};

    DivisionVertexBufferSettings vertex_buffer_settings = {
        .per_vertex_attributes = vertex_attrs,
        .per_instance_attributes = instance_attrs,
        .per_vertex_attribute_count = 3,
        .per_instance_attribute_count = 1,
        .vertex_count = vertex_count,
        .instance_count = instance_count,
        .index_count = index_count,
        .topology = DIVISION_TOPOLOGY_TRIANGLES,
    };

    uint32_t vertex_buffer;
    division_engine_vertex_buffer_alloc(ctx, &vertex_buffer_settings, &vertex_buffer);

    DivisionVertexBufferBorrowedData vb_borrowed;
    division_engine_vertex_buffer_borrow_data(ctx, vertex_buffer, &vb_borrowed);

    memcpy(vb_borrowed.vertex_data_ptr, vd, sizeof(vd));
    memcpy(vb_borrowed.instance_data_ptr, local_to_word_mat, sizeof(local_to_word_mat));
    memcpy(vb_borrowed.index_data_ptr, indices, sizeof(indices));

    division_engine_vertex_buffer_return_data(ctx, vertex_buffer, &vb_borrowed);

    *out_vertex_buffer_id = vertex_buffer;
    *out_vertex_count = vertex_count;
    *out_index_count = index_count;
    *out_instance_count = instance_count;
}

void example_create_uniform_buffer(
    DivisionContext* ctx, DivisionIdWithBinding* out_uniform_buffer
)
{
    float testVec[] = {0.5, 0.5, 0.5, 1};

    DivisionUniformBufferDescriptor buff = {.data_bytes = sizeof(testVec)};

    uint32_t uniform_buffer;
    division_engine_uniform_buffer_alloc(ctx, buff, &uniform_buffer);

    float* uniform_ptr =
        (float*)division_engine_uniform_buffer_borrow_data_pointer(ctx, uniform_buffer);
    memcpy(uniform_ptr, testVec, sizeof(testVec));
    division_engine_uniform_buffer_return_data_pointer(ctx, uniform_buffer, uniform_ptr);

    *out_uniform_buffer = (DivisionIdWithBinding){
        .id = uniform_buffer,
        .shader_location = 1,
    };
}

static void example_create_textures(
    DivisionContext* ctx, DivisionIdWithBinding* out_texture
)
{
    int image_width, image_height, channels_in_file;
    void* tex_data =
        stbi_load("example_font_bitmap.jpg", &image_width, &image_height, &channels_in_file, 3);

    DivisionTexture texture = {
        .texture_format = DIVISION_TEXTURE_FORMAT_RGB24Uint,
        .width = (uint32_t)image_width,
        .height = (uint32_t)image_height,
    };
    uint32_t tex_id;
    division_engine_texture_alloc(ctx, &texture, &tex_id);
    division_engine_texture_set_data(ctx, tex_id, tex_data);
    stbi_image_free(tex_data);

    *out_texture = (DivisionIdWithBinding){
        .id = tex_id,
        .shader_location = 0,
    };
}

static void example_write_font_character(DivisionContext* ctx)
{
    void* font_file;
    size_t file_size;
    uint32_t font_id, bitmap_id;
    DivisionFontGlyph glyph;
    const char32_t* characters = U"Su";

    assert(division_engine_font_alloc(ctx, "./Roboto-Medium.ttf", 64, &font_id));
    assert(division_engine_font_get_glyph(ctx, font_id, U'X', &glyph));

    void* bitmap = malloc(glyph.width * glyph.height);

    assert(bitmap);
    assert(division_engine_font_rasterize_glyph(ctx, font_id, &glyph, bitmap));
    
    assert(stbi_write_jpg(
        "example_font_bitmap.jpg", 
        glyph.width, 
        glyph.height, 
        1, 
        bitmap, 
        100
    ) > 0);
}