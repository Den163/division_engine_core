#include "division_engine_core/context.h"
#include "division_engine_core/platform_internal/platform_texture.h"

#include <stdlib.h>

#include "division_engine_core/texture.h"
#include "glfw_texture.h"

typedef struct
{
    GLenum gl_sized_internal_format;
    GLenum gl_internal_format;
} GlTextureTraits_;

static inline bool get_texture_traits(
    DivisionContext* ctx,
    DivisionTextureFormat texture_format,
    GlTextureTraits_* out_traits
);

bool division_engine_internal_platform_texture_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->texture_context->textures_impl = NULL;
    return true;
}
void division_engine_internal_platform_texture_context_free(DivisionContext* ctx)
{
    for (int i = 0; i < ctx->texture_context->texture_count; i++)
    {
        division_engine_internal_platform_texture_free(ctx, i);
    }
    free(ctx->texture_context->textures_impl);
}

bool division_engine_internal_platform_texture_realloc(
    DivisionContext* ctx, size_t new_size
)
{
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;
    tex_ctx->textures_impl =
        realloc(tex_ctx->textures_impl, sizeof(DivisionTextureImpl_[new_size]));

    return tex_ctx->textures_impl != NULL;
}

bool division_engine_internal_platform_texture_impl_init_new_element(
    DivisionContext* ctx, uint32_t texture_id
)
{
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;
    DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[texture_id];
    DivisionTexture* tex = &tex_ctx->textures[texture_id];

    GlTextureTraits_ gl_traits;
    if (!get_texture_traits(ctx, tex->texture_format, &gl_traits))
    {
        return false;
    }

    tex_impl->gl_texture_format = gl_traits.gl_internal_format;

    glCreateTextures(GL_TEXTURE_2D, 1, &tex_impl->gl_texture);
    glTextureStorage2D(
        tex_impl->gl_texture,
        1,
        gl_traits.gl_sized_internal_format,
        (GLsizei)tex->width,
        (GLsizei)tex->height
    );

    return true;
}

void division_engine_internal_platform_texture_set_data(
    DivisionContext* ctx, uint32_t texture_id, void* data
)
{
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;
    DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[texture_id];
    DivisionTexture* tex = &tex_ctx->textures[texture_id];

    glTextureSubImage2D(
        tex_impl->gl_texture,
        0,
        0,
        0,
        (GLsizei)tex->width,
        (GLsizei)tex->height,
        tex_impl->gl_texture_format,
        GL_UNSIGNED_BYTE,
        data
    );
}
void division_engine_internal_platform_texture_free(
    DivisionContext* ctx, uint32_t texture_id
)
{
    DivisionTextureImpl_* texture = &ctx->texture_context->textures_impl[texture_id];
    glDeleteTextures(1, &texture->gl_texture);
    texture->gl_texture = 0;
}

bool get_texture_traits(
    DivisionContext* ctx,
    DivisionTextureFormat texture_format,
    GlTextureTraits_* out_traits
)
{
    switch (texture_format)
    {
    case DIVISION_TEXTURE_FORMAT_R8Uint:
        *out_traits = (GlTextureTraits_){GL_R8, GL_RED};
        return true;
    case DIVISION_TEXTURE_FORMAT_RGB24Uint:
        *out_traits = (GlTextureTraits_){GL_RGB8, GL_RGB};
        return true;
    case DIVISION_TEXTURE_FORMAT_RGBA32Uint:
        *out_traits = (GlTextureTraits_){GL_RGBA8, GL_RGBA};
        return true;
    default:
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unknown texture format type");
        return false;
    }
}
