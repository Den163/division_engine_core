include(FetchContent)

FetchContent_Declare(
        glslang
        GIT_REPOSITORY https://github.com/KhronosGroup/glslang
        GIT_TAG 12.1.0
)
FetchContent_MakeAvailable(glslang)

add_library(division_engine_shader_compiler src/interface.cpp)
target_include_directories(division_engine_shader_compiler PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR})
target_link_libraries(division_engine_shader_compiler division_engine_core glslang SPIRV glslang-default-resource-limits)
