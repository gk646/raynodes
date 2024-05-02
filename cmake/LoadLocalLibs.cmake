set(DEPENDENCIES_PATH "C:/Users/gk646/Documents/Libaries")

include_directories(
        "${DEPENDENCIES_PATH}/raylib-5.0/src"
        "${DEPENDENCIES_PATH}/cxstructs/include" #Just supply a clone of cxstructs/include here
)

add_subdirectory("${DEPENDENCIES_PATH}/raylib-5.0" raylib)