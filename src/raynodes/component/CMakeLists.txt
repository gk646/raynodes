file(GLOB_RECURSE COMP_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(component STATIC ${COMP_FILES})
target_include_directories(component PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)