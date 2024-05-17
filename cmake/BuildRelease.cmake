set(CPACK_PACKAGE_NAME "raynodes")

if (MSVC)
    set(STAGING_DIR "${CMAKE_BINARY_DIR}/staging-WIN")
    set(OS "Windows")
    set(REDIS_PATH "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Redist/MSVC/14.38.33130/x64/Microsoft.VC143.CRT")
    set(EXE_NAME "raynodes.exe")
    set(LIB_NAME "raylib.dll")
else ()
    set(STAGING_DIR "${CMAKE_BINARY_DIR}/staging-UNIX")
    set(OS "GNU-Linux")
    set(REDIS_PATH "//wsl.localhost/Ubuntu-22.04/usr/lib/gcc/x86_64-linux-gnu/11")
    set(EXE_NAME "raynodes")
    set(LIB_NAME "raylib.so")
endif ()

# Ensure the staging directory is properly set up and all files are copied after the build
add_custom_command(TARGET raynodes POST_BUILD
        COMMENT "Packaging the application..."
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_DIR}/include"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_DIR}/res"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_DIR}/plugins"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:raynodes>" "${STAGING_DIR}/${EXE_NAME}"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:raylib>" "${STAGING_DIR}/${LIB_NAME}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/THIRD_PARTY_LICENSES" "${STAGING_DIR}/THIRD_PARTY_LICENSES"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/LICENSE" "${STAGING_DIR}/LICENSE"
)

# Redistributables
if (MSVC)
    set(UCRT_PATH "C:/Program Files (x86)/Windows Kits/10/Redist/10.0.22621.0/ucrt/DLLs/x64")
    add_custom_command(TARGET raynodes POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${REDIS_PATH}/msvcp140.dll" "${STAGING_DIR}/msvcp140.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${REDIS_PATH}/vcruntime140.dll" "${STAGING_DIR}/vcruntime140.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${REDIS_PATH}/concrt140.dll" "${STAGING_DIR}/concrt140.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${REDIS_PATH}/vccorlib140.dll" "${STAGING_DIR}/vccorlib140.dll"
            COMMAND ${CMAKE_COMMAND} -E copy "${UCRT_PATH}/ucrtbase.dll" "${STAGING_DIR}/ucrtbase.dll"
    )

elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_custom_command(TARGET raynodes POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${REDIS_PATH}/libgcc_s.so.1" "${STAGING_DIR}/libgcc_s.so.1"
            COMMAND ${CMAKE_COMMAND} -E copy "${REDIS_PATH}/libstdc++.so.6" "${STAGING_DIR}/libstdc++.so.6"
            COMMAND ${CMAKE_COMMAND} -E copy "${REDIS_PATH}/libc.so.6" "${STAGING_DIR}/libc.so.6"
            COMMAND ${CMAKE_COMMAND} -E copy "${REDIS_PATH}/libm.so.6" "${STAGING_DIR}/libm.so.6"
    )
endif ()

add_custom_command(TARGET raynodes POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/plugins" "${STAGING_DIR}/plugins"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/src/import/RnImport.h" "${STAGING_DIR}/include/RnImport.h"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/res" "${STAGING_DIR}/res"
        COMMAND ${CMAKE_COMMAND} -E echo "Files copied to staging directory."
        COMMAND ${CMAKE_COMMAND} -E echo "Zipping the package..."
        COMMAND ${CMAKE_COMMAND} -E chdir ${STAGING_DIR} ${CMAKE_COMMAND} -E tar "cfv" "${CMAKE_SOURCE_DIR}/${CPACK_PACKAGE_NAME}_${OS}_${RN_VERSION}.zip" --format=zip .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Zipping the package..."
        DEPENDS raynodes
)