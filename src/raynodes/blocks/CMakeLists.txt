file(GLOB_RECURSE BLOCKS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(blocks STATIC ${BLOCKS_FILES})
target_include_directories(blocks PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)