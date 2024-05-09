# Add catch 2 - the easy way
add_library(catch2 STATIC "${DEPENDENCIES_PATH}/catch2v3/catch_amalgamated.cpp")

# Collect all ".cpp" files
file(GLOB_RECURSE TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(raynodes_test ${TEST_FILES} "${CMAKE_SOURCE_DIR}/src/plugins/BuiltIns/components/TextOutC.cpp")
target_include_directories(raynodes_test PRIVATE "${CMAKE_SOURCE_DIR}/src/raynodes" "${DEPENDENCIES_PATH}/catch2v3") # We use the new version
target_link_libraries(raynodes_test PRIVATE catch2 raylib editor)

# Register the test with CMake - run from binary dir
add_test(NAME ImportTest COMMAND raynodes_test [Import] --benchmark-samples 5  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
add_test(NAME PersistTest COMMAND raynodes_test [Persist] --benchmark-samples 5  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})