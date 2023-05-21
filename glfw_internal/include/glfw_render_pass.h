#pragma once

#include "glad_restrict.h"
#include "division_engine_core/render_pass.h"

typedef struct DivisionRenderPassInternalPlatform_ {
    GLenum* gl_enables;
    GLenum* gl_disables;
    uint32_t gl_enable_count;
    uint32_t gl_disable_count;

    GLenum gl_blend_src;
    GLenum gl_blend_dst;
    GLenum gl_blend_equation;
} DivisionRenderPassInternalPlatform_;
