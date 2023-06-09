#pragma once

#include <Metal/Metal.h>

typedef struct DivisionTextureImpl_ {
    __strong id<MTLTexture> mtl_texture;
    __strong id<MTLSamplerState> mtl_sampler;
    uint32_t bytes_per_pixel;
    uint32_t src_data_bytes_per_pixel;
} DivisionTextureImpl_;