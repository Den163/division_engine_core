project(osx_metal_internal LANGUAGES OBJC)

set(OSX_METAL_INTERNAL_SOURCES
    src/osx_metal_renderer.m
    src/osx_metal_shader.m
    src/osx_metal_vertex_buffer.m
    src/DivisionOSXAppDelegate.m
    src/DivisionOSXViewDelegate.m
    src/osx_metal_uniform_buffer.m
    src/osx_render_pass.m)

add_library(osx_metal_internal STATIC ${OSX_METAL_INTERNAL_SOURCES})
set_target_properties(
        osx_metal_internal PROPERTIES
        COMPILE_FLAGS "-fobjc-arc"
)

target_include_directories(osx_metal_internal PRIVATE include ${CMAKE_SOURCE_DIR}/include ${CMAKE_BINARY_DIR})
target_link_libraries(osx_metal_internal
                      "-framework Foundation -framework AppKit -framework Metal -framework MetalKit")