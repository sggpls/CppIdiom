cmake_minimum_required(VERSION 3.2.0)

set(CMAKE_CXX_STANDARD 11)

# only for cmake 3.1 and newer
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin)

include_directories(${PROJECT_SOURCE_DIR})
add_subdirectory(smart_pointers)

add_subdirectory(lib/googletest/googletest)
enable_testing()
include_directories(
    ${gtest_SOURCE_DIR}/include
    ${gtest_SOURCE_DIR}
)

add_subdirectory(test)