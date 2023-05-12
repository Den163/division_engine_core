CMAKE_MINIMUM_REQUIRED(VERSION 3.22.2)

project(division_engine_core LANGUAGES C CXX OBJC)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

include(GenerateExportHeader)


if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
add_subdirectory(example)
add_subdirectory(tests)
endif()

set(SOURCES
    src/context.c include/division_engine_core/context.h
    src/renderer.c include/division_engine_core/renderer.h
    src/shader.c include/division_engine_core/shader.h
    src/vertex_buffer.c include/division_engine_core/vertex_buffer.h
    src/uniform_buffer.c include/division_engine_core/uniform_buffer.h
    src/render_pass.c include/division_engine_core/render_pass.h
    src/unordered_id_table.c include/division_engine_core/data_structures/unordered_id_table.h
    src/ordered_id_table.c include/division_engine_core/data_structures/ordered_id_table.h
)

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
