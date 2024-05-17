
set(DEPENDENCIES_PATH "${CMAKE_SOURCE_DIR}/src/external") # Internal library storage

include_directories(
        "${DEPENDENCIES_PATH}/raylib/src"
        "${DEPENDENCIES_PATH}/raygui/src"
        "${DEPENDENCIES_PATH}/tinyfiledialogs"
        "${DEPENDENCIES_PATH}/cxstructs/include"
)

add_subdirectory("${DEPENDENCIES_PATH}/raylib" raylib)

add_library(nativefiledialogs STATIC "${DEPENDENCIES_PATH}/tinyfiledialogs/tinyfiledialogs.c")