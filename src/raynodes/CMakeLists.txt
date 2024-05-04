# Collect all ".cpp" files (in an IDE this is just a reload)
file(GLOB_RECURSE EDITOR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
# Exclude main
list(FILTER EDITOR_FILES EXCLUDE REGEX "main.cpp")
# Build the library
add_library(editor STATIC ${EDITOR_FILES})
# Set include dir for full paths
target_include_directories(editor PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)
target_link_libraries(editor PRIVATE nativefiledialogs)
# Define the type
add_executable(raynodes main.cpp)
# This places the executable in the root of the build directory
set_target_properties(raynodes PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Link against user32 on windows
target_link_libraries(raynodes PUBLIC raylib user32 editor)