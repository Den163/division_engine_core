add_executable(division_example main.c lib/stb_image.h)

target_link_libraries(division_example PRIVATE division_engine_core)
target_include_directories(division_example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(EXAMPLE_RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.vert.metal
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.frag.metal
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.metal
    ${CMAKE_CURRENT_SOURCE_DIR}/images/nevsky.jpg
)

set(ENV{DIVISION_ENGINE_USE_SHADER_COMPILER} 1)

    message("Copying example shaders...")
    foreach(EXAMPLE_RES ${EXAMPLE_RESOURCES})
        configure_file(${EXAMPLE_RES} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
    endforeach()