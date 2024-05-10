# Collect all ".cpp" files (in an IDE this is just a reload)
file(GLOB_RECURSE EDITOR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# Exclude main
list(FILTER EDITOR_FILES EXCLUDE REGEX "main.cpp")
# Build the library
add_library(editor STATIC ${EDITOR_FILES})
# Set include dir for full paths
target_include_directories(editor PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)
target_link_libraries(editor PRIVATE nativefiledialogs)
# Define the main executable
add_executable(raynodes main.cpp)
target_include_directories(raynodes PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)
# This places the executable in the root of the build directory
set_target_properties(raynodes PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Link raylib and the core editor
target_link_libraries(raynodes PUBLIC raylib editor)

if (MSVC)
    target_sources(raynodes PRIVATE shared/app.rc) # Add the icon resource file
    target_link_libraries(raynodes PRIVATE user32) # Need to link to avoid duplicate symbols
    add_custom_command(TARGET raynodes POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_BINARY_DIR}/raylib/raylib/raylib.dll"
            "${CMAKE_BINARY_DIR}/raylib.dll"
            COMMENT "Moving raylib.dll to project binary directory"
    )

else ()
    add_custom_command(TARGET raynodes POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E rename
            "${CMAKE_BINARY_DIR}/raylib/raylib/libraylib.so"
            "${CMAKE_BINARY_DIR}/libraylib.so"
            COMMENT "Moving raylib.so to project binary directory"
    )
endif ()