#include "division_engine_shader_compiler/interface.h"

#include <cstdio>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>

static inline EShLanguage division_shader_type_to_glslang_type(DivisionShaderType shader_type);

bool division_engine_shader_compiler_alloc()
{
    return glslang::InitializeProcess();
}

bool division_engine_shader_compiler_compile_glsl_to_spirv(
    const char* source,
    DivisionShaderType shader_type,
    const char* spirv_entry_point_name,
    void** out_spirv,
    size_t* out_spirv_byte_count)
{

    EShLanguage glslang_shader_type = division_shader_type_to_glslang_type(shader_type);
    glslang::TShader shader {glslang_shader_type};
    const TBuiltInResource* default_resource = GetDefaultResources();

    shader.setEntryPoint(spirv_entry_point_name);
    shader.setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EshTargetClientVersion::EShTargetOpenGL_450);
    shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_5);
    shader.setStrings(&source, 1);

    auto includer = glslang::TShader::ForbidIncluder {};
    std::string preprocess_output {};
    bool preprocess_ok = shader.preprocess(
        default_resource,
        450,
        EProfile::ECoreProfile,
        true,
        false,
        EShMessages::EShMsgDefault,
        &preprocess_output,
        includer
    );

    if (!preprocess_ok) {
        const char* info_log = shader.getInfoLog();
        fprintf(stderr, "Failed to preprocess a shader. Log: %s\n", info_log);
        return false;
    }

    const char* preprocess_c_str = preprocess_output.c_str();
    shader.setStrings(&preprocess_c_str, 1);

    if (!shader.parse(default_resource, 450, false, EShMessages::EShMsgDefault))
    {
        const char* info_log = shader.getInfoLog();
        fprintf(stderr, "Failed to parse a shader. Log: %s\n", info_log);
        return false;
    }

    {
        glslang::TProgram program {};
        program.addShader(&shader);
        if (!program.link(EShMessages::EShMsgDefault))
        {
            const char* info_log = program.getInfoLog();
            fprintf(stderr, "Failed to link a program. Log: %s\n", info_log);
            return false;
        }

        const glslang::TIntermediate* intermediate = program.getIntermediate(glslang_shader_type);
        std::vector<uint32_t> spv {};
        glslang::SpvOptions options {
            .generateDebugInfo = false,
            .stripDebugInfo = true,
            .disableOptimizer = false,
            .optimizeSize = true,
            .disassemble = false,
            .validate = true,
            .emitNonSemanticShaderDebugInfo = false,
            .emitNonSemanticShaderDebugSource = false
        };

        spv::SpvBuildLogger logger {};
        glslang::GlslangToSpv(*intermediate, spv, &logger, &options);

        auto msg = logger.getAllMessages();

        if (!msg.empty())
        {
            printf("SPIRV log messages: \n%s\n", msg.c_str());
        }

        *out_spirv_byte_count = sizeof(uint32_t[spv.size()]);
        *out_spirv = malloc(*out_spirv_byte_count);
    }

    return true;
}

void division_engine_shader_compiler_free()
{
    glslang::FinalizeProcess();
}

EShLanguage division_shader_type_to_glslang_type(DivisionShaderType shader_type)
{
    switch (shader_type)
    {
        case DIVISION_SHADER_VERTEX:
            return EShLanguage::EShLangVertex;
        case DIVISION_SHADER_FRAGMENT:
            return EShLanguage::EShLangFragment;
        default:
            throw std::runtime_error("Unknown shader type");
    }
}