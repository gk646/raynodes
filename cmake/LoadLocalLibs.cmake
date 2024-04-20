set(DEPENDENCIES_PATH "C:/Users/gk646/Documents/Libaries")

include_directories(
        "${DEPENDENCIES_PATH}/raylib-master/src"
        "${CMAKE_SOURCE_DIR}/../cxstructs/src" #Just supply a clone of cxstructs/include here
)

add_subdirectory("${DEPENDENCIES_PATH}/raylib-master" raylib)