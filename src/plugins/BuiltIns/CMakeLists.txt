set(PLUGIN_NAME BuiltIns)
file(GLOB_RECURSE PLUGIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
include(${CMAKE_SOURCE_DIR}/cmake/PluginDefinition.cmake)