# IMPORTANT: Include a CMakeLists.txt with this in your plugin directory

#set(PLUGIN_NAME MyPluginName)
#file(GLOB_RECURSE PLUGIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
#include(${CMAKE_SOURCE_DIR}/cmake/PluginDefinition.cmake)


# Add all the plugins
add_subdirectory(BuiltIns) # Built in Types (raynodes STL...)
add_subdirectory(QuestScript) # Quest Script
add_subdirectory(Logic) # Logic (circuits)