if(MSVC)
    # Use dynamic linking for the runtime library on Windows
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
elseif(UNIX)
endif()