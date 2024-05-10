
set(DEPENDENCIES_PATH "C:/Users/gk646/Documents/Libraries") # Set to where your libraries are stored

include_directories(
        "${DEPENDENCIES_PATH}/raylib-master/src"  # Supply a clone of raylib.git
        "${DEPENDENCIES_PATH}/raygui-master/src"  # Supply a clone of raygui.git
        "${DEPENDENCIES_PATH}/tinyfiledialogs" # Download from here https://sourceforge.net/projects/tinyfiledialogs/
        "${DEPENDENCIES_PATH}/cxstructs-master/include" # Supply a clone of cxstructs.git
)

add_subdirectory("${DEPENDENCIES_PATH}/raylib-master" raylib)


add_library(nativefiledialogs STATIC "${DEPENDENCIES_PATH}/tinyfiledialogs/tinyfiledialogs.c")