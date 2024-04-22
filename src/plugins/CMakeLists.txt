# Plugins

# "CMakeLists.txt" template for plugins:
#
# add_library(MyPlugin SHARED MQQS.cpp)
#
#set_target_properties(MyPlugin PROPERTIES
#        PREFIX ""
#        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/plugins"
#        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/plugins"
#)
#
#target_include_directories(MyPlugin PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)

add_subdirectory(MQQS)