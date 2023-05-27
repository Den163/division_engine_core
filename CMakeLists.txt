CMAKE_MINIMUM_REQUIRED(VERSION 3.22.2)

project(division_engine_core)
set(CMAKE_C_STANDARD 11)

include(GenerateExportHeader)

if(DEFINED ENV{DIVISION_BUILD_EXAMPLE})
    add_subdirectory(example)
endif()

if (TARGET tests)
    add_subdirectory(tests)
endif()

set(SOURCES
    src/context.c
    src/renderer.c
    src/shader.c
    src/vertex_buffer.c
    src/uniform_buffer.c
    src/render_pass.c
    src/unordered_id_table.c
    src/ordered_id_table.c
    src/io_utility.c
    src/hash_table.c
    src/texture.c)

add_library(division_engine_core ${SOURCES})
target_include_directories(division_engine_core PUBLIC ${PROJECT_SOURCE_DIR}/include ${PROJECT_BINARY_DIR})
target_include_directories(division_engine_core PRIVATE ${PROJECT_SOURCE_DIR}/platform_internal_include)

GENERATE_EXPORT_HEADER(division_engine_core EXPORT_MACRO_NAME DIVISION_EXPORT)

if(APPLE)
    message("Target renderer is OSX with METAL")
    add_subdirectory(osx_metal_internal)
    target_link_libraries(division_engine_core PUBLIC osx_metal_internal)
elseif(UNIX OR WIN32)
    message("Target renderer is GLFW with OPENGL")
    add_subdirectory(glfw_internal)
    target_link_libraries(division_engine_core PUBLIC glfw_internal)
endif()
