file(GLOB_RECURSE EDITOR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(editor STATIC ${EDITOR_FILES})
target_include_directories(editor PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)