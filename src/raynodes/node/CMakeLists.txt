file(GLOB_RECURSE NODE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(node STATIC ${NODE_FILES})
target_include_directories(node PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)