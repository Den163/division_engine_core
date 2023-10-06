#pragma once

#include "division_engine_core/render_pass.h"
#include "division_engine_core_export.h"

#include "context.h"
#include "data_structures/unordered_id_table.h"
#include <stdint.h>

typedef enum DivisionTextureFormat
{
    DIVISION_TEXTURE_FORMAT_R8Uint = 1,
    DIVISION_TEXTURE_FORMAT_RGB24Uint = 2,
    DIVISION_TEXTURE_FORMAT_RGBA32Uint = 3,
} DivisionTextureFormat;

typedef enum DivisionTextureChannelSwizzleVariant
{
    DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_ZERO = 0,
    DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_ONE = 1,
    DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_RED = 2,
    DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_GREEN = 3,
    DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_BLUE = 4,
    DIVISION_TEXTURE_CHANNEL_SWIZZLE_VARIANT_ALPHA = 5,
} DivisionTextureChannelSwizzleVariant;

typedef union DivisionTextureChannelsSwizzle
{
    struct {
        DivisionTextureChannelSwizzleVariant red;
        DivisionTextureChannelSwizzleVariant green;
        DivisionTextureChannelSwizzleVariant blue;
        DivisionTextureChannelSwizzleVariant alpha;
    };
    DivisionTextureChannelSwizzleVariant values[4];
} DivisionTextureChannelsSwizzle;

typedef struct DivisionTexture
{
    DivisionTextureChannelsSwizzle channels_swizzle;
    DivisionTextureFormat texture_format;
    uint32_t width;
    uint32_t height;
    bool has_channels_swizzle;
} DivisionTexture;

struct DivisionTextureImpl_;

typedef struct DivisionTextureSystemContext
{
    DivisionUnorderedIdTable id_table;
    DivisionTexture* textures;
    struct DivisionTextureImpl_* textures_impl;

    uint32_t texture_count;
} DivisionTextureSystemContext;

bool division_engine_texture_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
);
void division_engine_texture_system_context_free(DivisionContext* ctx);

#ifdef __cplusplus
extern "C"
{
#endif

    DIVISION_EXPORT bool division_engine_texture_alloc(
        DivisionContext* ctx, const DivisionTexture* texture, uint32_t* out_texture_id
    );
    DIVISION_EXPORT void division_engine_texture_free(
        DivisionContext* ctx, uint32_t texture_id
    );

    DIVISION_EXPORT void division_engine_texture_set_data(
        DivisionContext* ctx, uint32_t texture_id, void* data
    );

#ifdef __cplusplus
}
#endif
