cmake_minimum_required(VERSION 3.22.1)
project(raynodes)
set(CMAKE_CXX_STANDARD 23)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(OPENGL_VERSION "4.3" CACHE STRING "OpenGL Version to build raylib with")
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE) # Build as shared lib

option(RN_ENABLE_SANITIZER "Set to compile with adress sanitizer (MSVC)") # set(RN_ENABLE_SANITIZER ON)
option(RN_BUILD_TESTS "Set to compile the test executable")               # set(RN_BUILD_TESTS ON)


# Compiler options for MSVC and GCC
include(cmake/CompilerOptions.cmake)

# Choose how the STL is linked
include(cmake/LinkSTL.cmake)

# Dependencies are included in the project - its only 14 mb
include(cmake/LoadInternalLibs.cmake)

# Add subdirectories for the main application components
add_subdirectory(src/raynodes)

# Add subdirectories for plugins
add_subdirectory(src/plugins)

# Add the import header
add_subdirectory(src/import)

# Add all built targets here
add_dependencies(raynodes MQQS BuiltIns)

if (RN_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()