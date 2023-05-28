#include "division_engine_core/platform_internal/platform_texture.h"

#include "division_engine_core/renderer.h"
#include "division_engine_core/texture.h"

#include "osx_texture.h"
#include "osx_window_context.h"

typedef struct
{
    MTLPixelFormat pixel_format;
    int32_t mtl_bytes_per_pixel;
    int32_t src_data_bytes_per_pixel;
} DivisionMTLTexTraits_;

static inline bool try_get_texture_traits(
    DivisionContext* ctx, DivisionTextureFormat texture_format, DivisionMTLTexTraits_* out_traits);

bool division_engine_internal_platform_texture_context_alloc(DivisionContext* ctx, const DivisionSettings* settings)
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

bool division_engine_internal_platform_texture_realloc(DivisionContext* ctx, size_t new_size)
{
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;
    tex_ctx->textures_impl = realloc(tex_ctx->textures_impl, sizeof(DivisionTextureImpl_[new_size]));

    return tex_ctx->textures_impl != NULL;
}

bool division_engine_internal_platform_texture_impl_init_new_element(DivisionContext* ctx, uint32_t texture_id)
{
    id <MTLDevice> device = ctx->renderer_context->window_data->app_delegate->viewDelegate->device;
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;
    DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[texture_id];
    DivisionTexture* tex = &tex_ctx->textures[texture_id];

    DivisionMTLTexTraits_ traits;
    if (!try_get_texture_traits(ctx, tex->texture_format, &traits))
    {
        return false;
    }

    @autoreleasepool
    {
        MTLSamplerDescriptor* sample_desc = [MTLSamplerDescriptor new];
        MTLTextureDescriptor* tex_desc = [MTLTextureDescriptor new];
        tex_desc.width = tex->width;
        tex_desc.height = tex->height;

        tex_impl->mtl_texture = [device newTextureWithDescriptor:tex_desc];
        tex_impl->mtl_sampler = [device newSamplerStateWithDescriptor:sample_desc];
    }
    tex_impl->bytes_per_pixel = traits.mtl_bytes_per_pixel;
    tex_impl->src_data_bytes_per_pixel = traits.src_data_bytes_per_pixel;

    return true;
}

void division_engine_internal_platform_texture_free(DivisionContext* ctx, uint32_t texture_id)
{
    DivisionTextureImpl_* t = &ctx->texture_context->textures_impl[texture_id];
    t->mtl_texture = nil;
}

void division_engine_internal_platform_texture_set_data(DivisionContext* ctx, uint32_t texture_id, void* data)
{
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;
    DivisionTexture* tex = &tex_ctx->textures[texture_id];
    DivisionTextureImpl_* tex_impl = &tex_ctx->textures_impl[texture_id];
    uint32_t src_data_bytes_per_pixel = tex_impl->src_data_bytes_per_pixel;
    uint32_t bytes_per_row = tex_impl->bytes_per_pixel * tex->width;

    if (tex_impl->bytes_per_pixel == src_data_bytes_per_pixel)
    {
        MTLRegion region = {{0,          0,           0},
                            {tex->width, tex->height, 1}};

        [tex_impl->mtl_texture replaceRegion:region mipmapLevel:0 withBytes:data bytesPerRow:bytes_per_row];
    }
    else
    {
        int width = (int) tex->width,
            height = (int) tex->height;
        int texels = width * height;
        MTLRegion region = {{0, 0, 0},
                            {0, 0, 1}};
        for (int i = 0; i < texels; i++)
        {
            int xOffset = i % width;
            int yOffset = i / width;
            region.size.width = region.size.height = 1;
            region.origin.x = xOffset;
            region.origin.y = yOffset;

            void* src_ptr = data + i * src_data_bytes_per_pixel;
            [tex_impl->mtl_texture replaceRegion:region mipmapLevel:0 withBytes:src_ptr bytesPerRow:bytes_per_row];
        }
    }
}

bool try_get_texture_traits(
    DivisionContext* ctx, DivisionTextureFormat texture_format, DivisionMTLTexTraits_* out_traits)
{
    switch (texture_format)
    {
        case DIVISION_TEXTURE_FORMAT_R8Uint:
            *out_traits = (DivisionMTLTexTraits_) {MTLPixelFormatR8Uint, 1, 1};
            return true;
        case DIVISION_TEXTURE_FORMAT_RGB24Uint:
            *out_traits = (DivisionMTLTexTraits_) {MTLPixelFormatRGBA8Uint, 4, 3};
            return true;
        case DIVISION_TEXTURE_FORMAT_RGBA32Uint:
            *out_traits = (DivisionMTLTexTraits_) {MTLPixelFormatRGBA8Uint, 4, 4};
            return true;
        default:
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Unknown texture format");
            return false;
    }
}