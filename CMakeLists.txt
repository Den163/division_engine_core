CMAKE_MINIMUM_REQUIRED(VERSION 3.22.2)
project(division_engine_core)
set(CMAKE_C_STANDARD 11)

add_subdirectory(example)

include(GenerateExportHeader)

set(SOURCES
    src/context.c
    src/renderer.c
    src/shader.c
    src/vertex_buffer.c
    src/uniform_buffer.c src/render_pass.c)

add_library(division_engine_core STATIC ${SOURCES})

target_include_directories(division_engine_core PUBLIC ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR})

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
