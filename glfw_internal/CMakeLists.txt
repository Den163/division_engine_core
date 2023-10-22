include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_VULKAN_STATIC OFF CACHE BOOL "" FORCE)

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
    src/glfw_shader.c src/glfw_uniform_buffer.c 
    src/glfw_render_pass_instance.c 
    src/glfw_render_pass_descriptor.c 
    src/glfw_texture.c
)

add_library(glfw_internal STATIC ${GLFW_INTERNAL_SOURCES})

target_include_directories(glfw_internal PRIVATE
                           ${PROJECT_SOURCE_DIR}/include
                           ${PROJECT_SOURCE_DIR}/platform_internal_include
                           ${CMAKE_CURRENT_SOURCE_DIR}/lib/glad/include
                           ${CMAKE_CURRENT_SOURCE_DIR}/include
                           ${PROJECT_BINARY_DIR})

target_link_libraries(glfw_internal PRIVATE glfw)
set(FILES_TO_COPY ${GLFW_BINARY_DIR}/src/libglfw3.a)

if(UNIX)
    include(GNUInstallDirs)
    install(TARGETS glfw_internal DESTINATION ${CMAKE_INSTALL_LIBDIR})
else()
    install(TARGETS glfw_internal DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("Enabled OpenGL debug output")
    target_compile_definitions(glfw_internal PUBLIC DIVISION_OPENGL_DEBUG)
endif()
