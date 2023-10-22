#include "division_engine_core/context.h"
#include "division_engine_core/platform_internal/platform_texture.h"

#include <stdbool.h>
#include <stdlib.h>

#include "division_engine_core/texture.h"
#include "glfw_texture.h"

typedef struct
{
    GLenum gl_sized_internal_format;
    GLenum gl_internal_format;
    int pixel_bytes_alignment;
} GlTextureTraits_;

static inline bool try_get_texture_traits(
    DivisionContext* ctx,
    DivisionTextureFormat texture_format,
    GlTextureTraits_* out_traits
);

static inline bool try_get_gl_swizzle_var(
    DivisionContext* ctx,
    DivisionTextureChannelSwizzleVariant swizzle,
    GLint* out_gl_swizzle
);

static inline bool try_get_filter(
    DivisionContext* ctx, DivisionTextureMinMagFilter filter, GLint* out_gl_filter
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
    if (!try_get_texture_traits(ctx, tex->texture_format, &gl_traits))
    {
        return false;
    }

    tex_impl->gl_texture_format = gl_traits.gl_internal_format;
    tex_impl->pixel_byte_alignment = gl_traits.pixel_bytes_alignment;

    glCreateTextures(GL_TEXTURE_2D, 1, &tex_impl->gl_texture);
    glTextureStorage2D(
        tex_impl->gl_texture,
        1,
        gl_traits.gl_sized_internal_format,
        (GLsizei)tex->width,
        (GLsizei)tex->height
    );

    if (tex->has_channels_swizzle)
    {
        DivisionTextureChannelsSwizzle tex_swizzle = tex->channels_swizzle;

        GLint swizzle_mask[4];
        for (int i = 0; i < 4; i++)
        {
            if (!try_get_gl_swizzle_var(ctx, tex_swizzle.values[i], &swizzle_mask[i]))
            {
                glDeleteTextures(1, &tex_impl->gl_texture);
                return false;
            }
        }

        glTextureParameteriv(tex_impl->gl_texture, GL_TEXTURE_SWIZZLE_RGBA, swizzle_mask);
    }

    DivisionTextureMinMagFilter filters[2] = { tex->min_filter, tex->mag_filter };
    GLint gl_filters[2];
    for (int i = 0; i < 2; i++)
    {
        if (!try_get_filter(ctx, filters[i], &gl_filters[i]))
        {
            return false;
        }
    }

    glTextureParameteri(tex_impl->gl_texture, GL_TEXTURE_MIN_FILTER, gl_filters[0]);
    glTextureParameteri(tex_impl->gl_texture, GL_TEXTURE_MAG_FILTER, gl_filters[1]);

    return true;
}

void division_engine_internal_platform_texture_set_data(
    DivisionContext* ctx, uint32_t texture_id, void* data
)
{
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;
    DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[texture_id];
    DivisionTexture* tex = &tex_ctx->textures[texture_id];

    glPixelStorei(GL_UNPACK_ALIGNMENT, tex_impl->pixel_byte_alignment);

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

bool try_get_texture_traits(
    DivisionContext* ctx,
    DivisionTextureFormat texture_format,
    GlTextureTraits_* out_traits
)
{
    switch (texture_format)
    {
    case DIVISION_TEXTURE_FORMAT_R8Uint:
        *out_traits = (GlTextureTraits_){GL_R8, GL_RED, 1};
        return true;
    case DIVISION_TEXTURE_FORMAT_RGB24Uint:
        *out_traits = (GlTextureTraits_){GL_RGB8, GL_RGB, 3};
        return true;
    case DIVISION_TEXTURE_FORMAT_RGBA32Uint:
        *out_traits = (GlTextureTraits_){GL_RGBA8, GL_RGBA, 4};
        return true;
    default:
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unknown texture format type");
        return false;
    }
}

bool try_get_gl_swizzle_var(
    DivisionContext* ctx,
    DivisionTextureChannelSwizzleVariant color_channel,
    GLint* out_variant
)
{
    switch (color_channel)
    {
    case DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_ZERO:
        *out_variant = GL_ZERO;
        return true;
    case DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_ONE:
        *out_variant = GL_ONE;
        return true;
    case DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_RED:
        *out_variant = GL_RED;
        return true;
    case DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_GREEN:
        *out_variant = GL_GREEN;
        return true;
    case DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_BLUE:
        *out_variant = GL_BLUE;
        return true;
    case DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_ALPHA:
        *out_variant = GL_ALPHA;
        return true;
    default:
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unknown swizzle variant type");
        return false;
    }
}

bool try_get_filter(
    DivisionContext* ctx, DivisionTextureMinMagFilter filter, GLint* out_gl_filter
)
{
    switch (filter) {
    case DIVISION_TEXTURE_MIN_MAG_FILTER_NEAREST:
        *out_gl_filter = GL_NEAREST;
        return true;
    case DIVISION_TEXTURE_MIN_MAG_FILTER_LINEAR:
        *out_gl_filter = GL_LINEAR;
        return true;
    default:
        DIVISION_THROW_INTERNAL_ERROR(ctx, "Unknown texture min mag filter type");
        return false;
    }
}