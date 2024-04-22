file(GLOB_RECURSE GRAPHICS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(graphics STATIC ${GRAPHICS_FILES})
target_include_directories(graphics PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)