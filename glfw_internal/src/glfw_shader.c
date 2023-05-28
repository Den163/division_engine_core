#include "division_engine_core/platform_internal/platfrom_shader.h"

#include <stdlib.h>
#include "glad/gl.h"
#include <stdio.h>

#include "division_engine_core/shader.h"
#include "glfw_shader.h"

static int create_shader_from_source(const char* source, size_t source_size, GLuint gl_shader_type);
static bool check_program_status(GLuint programHandle);
static void get_program_info_log(GLuint program_handle, char** error_ptr);
static int shader_type_to_gl_type(DivisionShaderType shaderType);

static inline bool attach_shader_to_program_from_source(
    const char* source, size_t source_size, DivisionShaderType type, int32_t program_id);

bool division_engine_internal_platform_shader_system_context_alloc(
    DivisionContext* ctx, const DivisionSettings* settings
)
{
    ctx->shader_context->shaders_impl = NULL;

    return true;
}

void division_engine_internal_platform_shader_system_context_free(DivisionContext* ctx)
{
    free(ctx->shader_context->shaders_impl);
}

bool division_engine_internal_platform_shader_program_alloc(
    DivisionContext* ctx,
    const DivisionShaderSourceDescriptor* settings,
    int32_t source_count,
    uint32_t* out_shader_program_id)
{
    int32_t gl_program = glCreateProgram();
    for (int i = 0; i < source_count; i++)
    {
        const DivisionShaderSourceDescriptor* s = &settings[i];
        attach_shader_to_program_from_source(s->source, s->source_size, s->type, gl_program);
    }
    glLinkProgram(gl_program);

    if (!check_program_status(gl_program))
    {

        ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to link shader program");
        glDeleteProgram(gl_program);
        return false;
    }

    DivisionShaderSystemContext* shader_ctx = ctx->shader_context;
    uint32_t program_id = division_unordered_id_table_insert(&ctx->shader_context->id_table);

    if (program_id >= shader_ctx->shader_count)
    {
        shader_ctx->shaders_impl = realloc(
            shader_ctx->shaders_impl,
            sizeof(DivisionShaderInternal_[program_id + 1])
        );

        if (shader_ctx->shaders_impl == NULL)
        {
            division_unordered_id_table_remove(&shader_ctx->id_table, program_id);
            ctx->error_callback(DIVISION_INTERNAL_ERROR, "Failed to realloc Shader Implementation array");
            return false;
        }

        shader_ctx->shader_count++;
    }

    shader_ctx->shaders_impl[program_id] = (DivisionShaderInternal_) {.gl_shader_program = gl_program};

    *out_shader_program_id = program_id;
    return true;
}

void division_engine_internal_platform_shader_program_free(DivisionContext* ctx, uint32_t shader_program_id)
{
    DivisionShaderInternal_* prog = &ctx->shader_context->shaders_impl[shader_program_id];

    glDeleteProgram(prog->gl_shader_program);
    prog->gl_shader_program = 0;

    division_unordered_id_table_remove(&ctx->shader_context->id_table, shader_program_id);
}

bool attach_shader_to_program_from_source(
    const char* source, size_t source_size, DivisionShaderType type, int32_t program_id)
{
    int gl_shader_type = shader_type_to_gl_type(type);
    if (gl_shader_type < 0)
    {
        return false;
    }

    int shader_handle = create_shader_from_source(source, source_size, gl_shader_type);
    if (shader_handle < 0)
    {
        return false;
    }

    glAttachShader((GLuint) program_id, shader_handle);
    glDeleteShader(shader_handle);
    return true;
}

int create_shader_from_source(const char* source, size_t source_size, GLuint gl_shader_type)
{
    GLuint shader_handle = glCreateShader(gl_shader_type);
    if (!shader_handle)
    {
        fprintf(stderr, "Failed to create a shader");
        return -1;
    }

    GLint int_src_size = (GLint) source_size;
    glShaderSource(shader_handle, 1, (const GLchar* const*) &source, &int_src_size);
    glCompileShader(shader_handle);

    GLint compile_result = 0;
    glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compile_result);
    if (compile_result == GL_TRUE)
    {
        return (int) shader_handle;
    }

    GLint error_length = 0;
    glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &error_length);
    char* error_log_data = malloc(error_length);
    glGetShaderInfoLog(shader_handle, error_length, &error_length, error_log_data);

    fprintf(stderr, "Failed to compile shader source. Info log: \n%s\n", error_log_data);
    free(error_log_data);
    return -1;
}

bool check_program_status(GLuint programHandle)
{
    GLint linkStatus;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        char* error;
        get_program_info_log(programHandle, &error);
        fprintf(stderr, "Failed to link a shader program. Info log: \n%s\n", error);
        free(error);
        return false;
    }

    glValidateProgram(programHandle);
    GLint validateStatus;
    glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE)
    {
        char* error;
        get_program_info_log(programHandle, &error);
        fprintf(stderr, "Failed to validate a shader program. Info log: \n%s\n", error);
        free(error);
        return false;
    }

    return true;
}

void get_program_info_log(GLuint program_handle, char** error_ptr)
{
    GLint error_length;
    glGetProgramiv(program_handle, GL_INFO_LOG_LENGTH, &error_length);
    char* error = malloc(error_length);
    glGetProgramInfoLog(program_handle, error_length, &error_length, error);

    *error_ptr = error;
}

int shader_type_to_gl_type(DivisionShaderType shaderType)
{
    switch (shaderType)
    {
        case DIVISION_SHADER_VERTEX:
            return GL_VERTEX_SHADER;
        case DIVISION_SHADER_FRAGMENT:
            return GL_FRAGMENT_SHADER;
        default:
            fprintf(stderr, "Unknown type of shader");
            return -1;
    }
}

