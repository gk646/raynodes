
set(OS "Windows")
set(CPACK_PACKAGE_NAME "raynodes")

set(STAGING_DIR "${CMAKE_BINARY_DIR}/staging")

# Ensure the staging directory is properly set up and all files are copied after the build
add_custom_command(TARGET raynodes POST_BUILD
        COMMENT "Packaging the application..."
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_DIR}/include"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_DIR}/res"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${STAGING_DIR}/plugins"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:raynodes>" "${STAGING_DIR}/raynodes.exe"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:raylib>" "${STAGING_DIR}/raylib.dll"
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