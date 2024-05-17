# This script expects PLUGIN_NAME and PLUGIN_FILES to be set before including it.

if(NOT DEFINED PLUGIN_NAME)
    message(FATAL_ERROR "PLUGIN_NAME is not defined")
    message("Use set(PLUGIN_NAME MyPlugin) to set the name")
endif()

if(NOT DEFINED PLUGIN_FILES)
    message(FATAL_ERROR "PLUGIN_FILES is not defined")
    message("Use set(PLUGIN_NAME MyPlugin) to set the name")

endif()

add_library(${PLUGIN_NAME} SHARED ${PLUGIN_FILES})

set_target_properties(${PLUGIN_NAME} PROPERTIES
        PREFIX ""
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins"
)

target_include_directories(${PLUGIN_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)
target_link_libraries(${PLUGIN_NAME} PRIVATE editor)