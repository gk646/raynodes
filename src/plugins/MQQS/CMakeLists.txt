add_library(MQQS SHARED MQQS.cpp)

set_target_properties(MQQS PROPERTIES
        PREFIX ""
        # Sets the output directory for the actual library files
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)


target_include_directories(MQQS PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)
target_link_libraries(MQQS PRIVATE raylib editor)