add_executable(division_example main.cpp)
target_link_libraries(division_example PRIVATE division_engine_core division_engine_shader_compiler)

set(EXAMPLE_SHADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/test.metal
)

if (TARGET division_example)
    message("Copying example shaders...")
    foreach(EXAMPLE_SHADER_FILE ${EXAMPLE_SHADERS})
        configure_file(${EXAMPLE_SHADER_FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
    endforeach()
endif()