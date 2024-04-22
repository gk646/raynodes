add_library(MQQS SHARED MQQS.cpp)

# Specify the output name of the library to remove any prefix ("lib" on Unix-based OS)
set_target_properties(MQQS PROPERTIES PREFIX "")

target_include_directories(MQQS PRIVATE ${CMAKE_SOURCE_DIR}/src/raynodes)

# target_link_libraries(MQQS some_other_library)