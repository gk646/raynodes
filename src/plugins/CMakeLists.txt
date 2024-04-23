# Plugins

# "CMakeLists.txt" template for plugins:
#
# add_library(MyPlugin SHARED MyPlugin.cpp)
#
#set_target_properties(MyPlugin PROPERTIES
#        PREFIX ""
#        # Sets the output directory for the actual library files
#        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
#        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
#)
#
#
#target_include_directories(MyPlugin PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)

add_subdirectory(MQQS) # MageQuest Quest Script
add_subdirectory(BuiltIns) # Built in Types (STL)