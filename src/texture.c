#include "division_engine_core/texture.h"
#include "division_engine_core/platform_internal/platform_texture.h"

#include <stdlib.h>

bool division_engine_internal_texture_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->texture_context =
        (DivisionTextureSystemContext*)malloc(sizeof(DivisionTextureSystemContext));
    ctx->texture_context->textures = NULL;
    ctx->texture_context->texture_count = 0;
    division_unordered_id_table_alloc(&ctx->texture_context->id_table, 10);

    return division_engine_internal_platform_texture_context_alloc(ctx, settings);
}

void division_engine_internal_texture_context_free(DivisionContext* ctx)
{
    division_engine_internal_platform_texture_context_free(ctx);
    division_unordered_id_table_free(&ctx->texture_context->id_table);

    free(ctx->texture_context->textures);
    free(ctx->texture_context);
}

bool division_engine_texture_alloc(
    DivisionContext* ctx, const DivisionTexture* texture, uint32_t* out_texture_id
)
{
    DivisionTextureSystemContext* tex_ctx = ctx->texture_context;
    uint32_t tex_id = division_unordered_id_table_insert(&tex_ctx->id_table);
    if (tex_id >= tex_ctx->texture_count)
    {
        size_t new_size = tex_id + 1;
        tex_ctx->textures = realloc(tex_ctx->textures, sizeof(DivisionTexture[new_size]));

        if (tex_ctx->textures == NULL ||
            !division_engine_internal_platform_texture_realloc(ctx, new_size))
        {
            ctx->lifecycle.error_callback(
                ctx, DIVISION_INTERNAL_ERROR, "Failed to realloc new textures buffers"
            );
            return false;
        }
    }

    tex_ctx->textures[tex_id] = *texture;
    *out_texture_id = tex_id;
    return division_engine_internal_platform_texture_impl_init_new_element(ctx, tex_id);
}

void division_engine_texture_free(DivisionContext* ctx, uint32_t texture_id)
{
    division_engine_internal_platform_texture_free(ctx, texture_id);
    division_unordered_id_table_remove(&ctx->texture_context->id_table, texture_id);
}

void division_engine_texture_set_data(
    DivisionContext* ctx, uint32_t texture_id, void* data
)
{
    division_engine_internal_platform_texture_set_data(ctx, texture_id, data);
}
