#pragma once

#include "glad_restrict.h"

typedef struct DivisionVertexBufferInternalPlatform_
{
    GLuint gl_vao;
    GLuint gl_vbo;
    GLuint gl_index_buffer;
    GLenum gl_topology;
} DivisionVertexBufferInternalPlatform_;
