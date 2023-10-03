#pragma once

#include "glad_restrict.h"

typedef struct DivisionTextureImpl_
{
    GLuint gl_texture;
    GLenum gl_texture_format;
    int pixel_byte_alignment;
} DivisionTextureImpl_;
