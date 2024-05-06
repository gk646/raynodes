file(GLOB_RECURSE PLUGIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

add_library(MQQS SHARED ${PLUGIN_FILES})

set_target_properties(MQQS PROPERTIES
        PREFIX ""
        # Sets the output directory for the actual library files
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)


target_include_directories(MQQS PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)
target_link_libraries(MQQS PRIVATE raylib editor)