file(GLOB_RECURSE PLUGIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(plugin STATIC ${PLUGIN_FILES})
target_include_directories(plugin PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)