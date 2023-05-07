add_library(metal metal.cpp)
target_include_directories(metal PUBLIC metal-cpp metal-cpp-extensions)

target_link_libraries(metal PUBLIC
  "-framework AppKit"
  "-framework MetalKit"
  "-framework Metal"
  "-framework Foundation"
  "-framework QuartzCore"
)