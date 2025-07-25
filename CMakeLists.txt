cmake_minimum_required(VERSION 3.10)
project(interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/core
    ${PROJECT_SOURCE_DIR}/include/ast
    ${PROJECT_SOURCE_DIR}/include/objects
)

file(GLOB_RECURSE SOURCES
    src/core/*.cpp
    src/ast/*.cpp
    src/objects/*.cpp
    src/Main.cpp
)

add_executable(interpreter ${SOURCES})
