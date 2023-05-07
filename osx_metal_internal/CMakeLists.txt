set(CMAKE_CXX_STANDARD 17)

set(OSX_METAL_INTERNAL_SOURCES
    src/osx_metal_renderer.cpp
    src/osx_metal_shader.cpp
    src/osx_metal_vertex_buffer.cpp
    src/DivisionOSXAppDelegate.cpp
    src/DivisionOSXViewDelegate.cpp
    src/osx_metal_uniform_buffer.cpp src/osx_render_pass.cpp)

add_library(osx_metal_internal STATIC ${OSX_METAL_INTERNAL_SOURCES})
add_subdirectory(lib/metal)

target_include_directories(osx_metal_internal PRIVATE include ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR})
target_link_libraries(osx_metal_internal PRIVATE metal)
