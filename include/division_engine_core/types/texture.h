#pragma once

#include <stdbool.h>
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

typedef enum DivisionTextureMinMagFilter 
{
    DIVISION_TEXTURE_MIN_MAG_FILTER_NEAREST = 1,
    DIVISION_TEXTURE_MIN_MAG_FILTER_LINEAR = 2
} DivisionTextureMinMagFilter;

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
    DivisionTextureMinMagFilter min_filter;
    DivisionTextureMinMagFilter mag_filter;
    uint32_t width;
    uint32_t height;
    bool has_channels_swizzle;
} DivisionTexture;