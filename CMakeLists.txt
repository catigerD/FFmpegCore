cmake_minimum_required(VERSION 3.10)
project(FFmpegCore)

set(CMAKE_CXX_STANDARD 11)

set(WORK_SPACE ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC ${WORK_SPACE}/src)

include_directories(${WORK_SPACE})
include_directories(${SRC})

file(GLOB FILES_WORK_SPACE ${WORK_SPACE}/*.cpp)
file(GLOB FILES_SRC ${SRC}/*.cpp)

add_executable(FFmpegCore ${FILES_WORK_SPACE} ${FILES_SRC})