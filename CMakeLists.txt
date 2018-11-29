cmake_minimum_required(VERSION 3.2.0)

set(CMAKE_CXX_STANDARD 11)

# only for cmake 3.1 and newer
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++11)
set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin)

add_subdirectory(smart_pointers)

set(SOURCES main.cpp)

add_executable(initial_executable ${SOURCES})

