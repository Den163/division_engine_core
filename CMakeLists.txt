CMAKE_MINIMUM_REQUIRED(VERSION 3.22.2)
project(division_engine_core)
set(CMAKE_C_STANDARD 11)

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

add_executable(division_main_test tests/main.c)
target_link_libraries(division_main_test PUBLIC division_engine_core)
target_include_directories(division_main_test PUBLIC include)

set(TEST_FILES_TO_COPY
    ${CMAKE_SOURCE_DIR}/tests/shaders/test.frag
    ${CMAKE_SOURCE_DIR}/tests/shaders/test.vert
    ${CMAKE_SOURCE_DIR}/tests/shaders/test.metal
)
foreach(TEST_COPY_FILE ${TEST_FILES_TO_COPY})
    configure_file(${TEST_COPY_FILE} ${PROJECT_BINARY_DIR} COPYONLY)
endforeach()


foreach (COPY_FILE ${FILES_TO_COPY})
    add_custom_command(TARGET division_engine_core POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${COPY_FILE} ${PROJECT_BINARY_DIR}
            )
endforeach ()

if(APPLE)
    message("Target renderer is OSX with METAL")
    add_subdirectory(osx_metal_internal)
    target_link_libraries(division_engine_core PUBLIC osx_metal_internal)
elseif(UNIX OR WIN32)
    message("Target renderer is GLFW with OPENGL")
    add_subdirectory(glfw_internal)
    target_link_libraries(division_engine_core PUBLIC glfw_internal)
endif()
