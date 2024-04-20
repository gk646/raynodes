include(FetchContent)

FetchContent_Declare(cxstructs
        GIT_REPOSITORY https://github.com/gk646/cxstructs.git
        GIT_TAG master
)
FetchContent_MakeAvailable(cxstructs)
FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
)
FetchContent_MakeAvailable(raylib)