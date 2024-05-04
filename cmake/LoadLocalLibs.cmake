set(DEPENDENCIES_PATH "C:/Users/Lukas/Documents/Libaries")

include_directories(
        "${DEPENDENCIES_PATH}/raylib-master/src"
        "${DEPENDENCIES_PATH}/raygui-master/src"
        "${DEPENDENCIES_PATH}/tinyfiledialogs"
        "${DEPENDENCIES_PATH}/cxstructs/include" #Just supply a clone of cxstructs/include here
)

add_subdirectory("${DEPENDENCIES_PATH}/raylib-master" raylib)

add_library(nativefiledialogs STATIC "${DEPENDENCIES_PATH}/tinyfiledialogs/tinyfiledialogs.c")