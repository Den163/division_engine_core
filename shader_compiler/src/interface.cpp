#include "division_shader_compiler/interface.h"

#include <cstdio>
#include <glslang/Public/ShaderLang.h>
#include <glslang/Public/ResourceLimits.h>
#include <SPIRV/GlslangToSpv.h>
#include <spirv_msl.hpp>

static inline EShLanguage division_shader_type_to_glslang_type(DivisionShaderType shader_type);
static inline spv::ExecutionModel division_shader_type_to_spv_cross_type(DivisionShaderType shader_type);

bool division_shader_compiler_alloc()
{
    return glslang::InitializeProcess();
}

bool division_shader_compiler_compile_glsl_to_spirv(
    const char* source, int32_t source_size,
    DivisionShaderType shader_type,
    const char* spirv_entry_point_name,
    void** out_spirv,
    size_t* out_spirv_byte_count)
{
    EShLanguage glslang_shader_type;
    try
    {
        glslang_shader_type = division_shader_type_to_glslang_type(shader_type);
    }
    catch (std::runtime_error& e)
    {
        fprintf(stderr, "%s\n", e.what());
        return false;
    }

    glslang::TShader shader {glslang_shader_type};
    const TBuiltInResource* default_resource = GetDefaultResources();

    shader.setPreamble("#extension GL_GOOGLE_include_directive: enable\n");
    shader.setEnvInput(glslang::EShSource::EShSourceGlsl, glslang_shader_type, glslang::EShClientOpenGL, 450);
    shader.setEnvClient(glslang::EShClient::EShClientOpenGL, glslang::EshTargetClientVersion::EShTargetOpenGL_450);
    shader.setEnvTarget(glslang::EShTargetLanguage::EShTargetSpv, glslang::EShTargetLanguageVersion::EShTargetSpv_1_5);
    shader.setStringsWithLengths(&source, &source_size, 1);
    if (spirv_entry_point_name)
    {
        shader.setSourceEntryPoint("main");
        shader.setEntryPoint(spirv_entry_point_name);
    }

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
            .stripDebugInfo = false,
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

        size_t spv_size = sizeof(uint32_t[spv.size()]);
        void* spv_bytes = malloc(spv_size);
        memcpy(spv_bytes, spv.data(), spv_size);

        *out_spirv = spv_bytes;
        *out_spirv_byte_count = spv_size;
    }

    return true;
}

bool division_shader_compiler_compile_spirv_to_metal(
    const void* spirv_bytes, size_t spirv_byte_count,
    DivisionShaderType shader_type, const char* entry_point,
    char** out_metal_source, size_t* out_metal_size)
{
    try
    {
        size_t spv_size = spirv_byte_count / sizeof(uint32_t);
        spirv_cross::CompilerMSL msl { (const uint32_t*) spirv_bytes, spv_size };
        auto entry_points = msl.get_entry_points_and_stages();
        for (auto& e : entry_points)
        {
            printf("Entry point: %s\n", e.name.c_str());
        }

        if (entry_point)
        {
            msl.set_entry_point(entry_point, division_shader_type_to_spv_cross_type(shader_type));
        }
        spirv_cross::CompilerMSL::Options opt {
            .msl_version = spirv_cross::CompilerMSL::Options::make_msl_version(3, 0),
            .enable_decoration_binding = true,
        };

        msl.set_msl_options(opt);
        std::string out_metal = msl.compile();

        size_t src_size = out_metal.size() + 1;
        char* src = static_cast<char*>(malloc(src_size));
        memcpy(src, out_metal.c_str(), src_size);

        *out_metal_source = src;
        *out_metal_size = src_size;
        return true;
    }
    catch (std::runtime_error& e)
    {
        fprintf(stderr, "Error while compiling spirv to msl: %s\n", e.what());
        return false;
    }
}

void division_shader_compiler_free()
{
    glslang::FinalizeProcess();
}

EShLanguage division_shader_type_to_glslang_type(DivisionShaderType shader_type)
{
    switch (shader_type)
    {
        case DIVISION_SHADER_VERTEX: return EShLanguage::EShLangVertex;
        case DIVISION_SHADER_FRAGMENT: return EShLanguage::EShLangFragment;
        default:
            throw std::runtime_error("Unknown shader typ to EShLanguage mapping");
    }
}

spv::ExecutionModel division_shader_type_to_spv_cross_type(DivisionShaderType shader_type)
{
    switch (shader_type)
    {
        case DIVISION_SHADER_VERTEX: return spv::ExecutionModel::ExecutionModelVertex;
        case DIVISION_SHADER_FRAGMENT: return spv::ExecutionModel::ExecutionModelFragment;
        default:
            throw std::runtime_error("Unknown shader type to spv::ExecutionModel mapping");
    }
}
