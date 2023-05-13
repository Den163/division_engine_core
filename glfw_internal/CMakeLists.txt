include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.3.8
)
FetchContent_MakeAvailable(glfw)


set(GLFW_INTERNAL_SOURCES
    lib/glad/include/glad/gl.h
    src/glfw_renderer.c
    src/glfw_vertex_buffer.c
    src/glfw_shader.c src/glfw_uniform_buffer.c include/glfw_uniform_buffer.h src/glfw_render_pass.c)

add_library(glfw_internal STATIC ${GLFW_INTERNAL_SOURCES})

target_include_directories(glfw_internal PRIVATE
                           ${PROJECT_SOURCE_DIR}/include
                           ${PROJECT_SOURCE_DIR}/platform_internal_include
                           ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
                           ${PROJECT_BINARY_DIR})

target_link_libraries(glfw_internal PRIVATE glfw ${GLFW_STATIC_LIBRARIES})
set(FILES_TO_COPY ${GLFW_BINARY_DIR}/src/libglfw3.a)