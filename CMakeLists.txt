cmake_minimum_required(VERSION 3.28)
project(raynodes)
set(CMAKE_CXX_STANDARD 23)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
set(OPENGL_VERSION "4.3" CACHE STRING "OpenGL Version to build raylib with")


set(DEPENDENCIES_PATH "C:/Users/Lukas/Documents/Libaries")

include_directories(
        "${DEPENDENCIES_PATH}/raylib-5.0/src"
        "C:/Users/Lukas/Documents/Libaries/cxstructs/include" #Just supply a clone of cxstructs/include here
        "${CMAKE_SOURCE_DIR}/src"
)


if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -ffast-math -fno-exceptions -fno-rtti)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Og -g")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Og -g")

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ffast-math -O3 -fno-exceptions -fno-rtti")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -flto -fno-exceptions -fno-rtti -ffast-math -O3")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
elseif (MSVC)
    add_compile_options(/W4 /fp:fast /EHsc- /GR-)
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Od")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od")

    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /O2 /EHsc- /GR-")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /LTCG /O2 /Ob3 /GA /Gw /GL /EHsc /GR- /arch:AVX2 /GF")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG /OPT:REF /OPT:ICF")
endif ()

add_compile_definitions(_HAS_EXCEPTIONS=0)

add_subdirectory("${DEPENDENCIES_PATH}/raylib-5.0" raylib)
add_subdirectory(src)


add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC raylib user32 editor node component blocks graphics)