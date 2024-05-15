# Collect all ".cpp" files for the editor (excluding main.cpp)
file(GLOB_RECURSE EDITOR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
list(FILTER EDITOR_FILES EXCLUDE REGEX "main.cpp")

# Build the core editor library
add_library(editor SHARED ${EDITOR_FILES})
target_include_directories(editor PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)
target_link_libraries(editor PUBLIC raylib nativefiledialogs)
set_target_properties(editor PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# Define the main executable
add_executable(raynodes main.cpp)
target_include_directories(raynodes PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)
set_target_properties(raynodes PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_link_libraries(raynodes PUBLIC editor)


if (MSVC)
    target_sources(raynodes PRIVATE shared/app.rc) # Add the icon resource file
    target_link_libraries(raynodes PRIVATE user32) # Need to link to avoid duplicate symbols
    add_custom_command(TARGET raynodes POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/raylib/raylib/raylib.dll"
            "${CMAKE_BINARY_DIR}/raylib.dll"
            COMMENT "Moving raylib.dll to project binary directory"
    )

else ()
    add_custom_command(TARGET raynodes POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/raylib/raylib/libraylib.so"
            "${CMAKE_BINARY_DIR}/libraylib.so"
            COMMENT "Moving raylib.so to project binary directory"
    )
endif ()


if (RN_BUILD_RELEASE)
    include(${CMAKE_SOURCE_DIR}/cmake/BuildRelease.cmake)
endif ()