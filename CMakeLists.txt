cmake_minimum_required(VERSION 3.28)
project(raynodes)
set(CMAKE_CXX_STANDARD 23)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(OPENGL_VERSION "4.3" CACHE STRING "OpenGL Version to build raylib with")
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE) # Build as shared lib


include(cmake/LinkSTL.cmake)

# Choose either one - currently only local libs supported
# include(cmake/FetchLibs.cmake)
include(cmake/LoadLocalLibs.cmake)

# Compiler options for MSVC and unix
include(cmake/CompilerOptions.cmake)

# Add subdirectories for the main application components
add_subdirectory(src/raynodes)

# Add subdirectories for plugins
add_subdirectory(src/plugins)

# Add all built targets here
add_dependencies(raynodes MQQS BuiltIns)