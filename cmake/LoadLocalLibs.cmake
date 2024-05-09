set(DEPENDENCIES_PATH "/mnt/c/Users/gk646/Documents/Libaries")

include_directories(
        "${DEPENDENCIES_PATH}/raylib-master/src"
        "${DEPENDENCIES_PATH}/raygui-master/src"
        "${DEPENDENCIES_PATH}/tinyfiledialogs"
        "${CMAKE_SOURCE_DIR}/../cxstructs/src" #Just supply a clone of cxstructs/include here
)

add_subdirectory("${DEPENDENCIES_PATH}/raylib-master" raylib)


add_library(nativefiledialogs STATIC "${DEPENDENCIES_PATH}/tinyfiledialogs/tinyfiledialogs.c")