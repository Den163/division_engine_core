#pragma once

#include <Metal/Metal.h>

typedef struct DivisionRenderPassInternalPlatform_ {
    __strong id<MTLRenderPipelineState> mtl_pipeline_state;
} DivisionRenderPassInternalPlatform_;
