cmake_minimum_required(VERSION 3.10)
project(FFmpegCore)

set(CMAKE_CXX_STANDARD 11)

set(WORK_SPACE ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC ${WORK_SPACE}/src)
set(MEDIA_DATA ${SRC}/media_data)
set(UTILS ${SRC}/utils)

include_directories(${WORK_SPACE})
include_directories(${SRC})
include_directories(${MEDIA_DATA})
include_directories(${UTILS})

file(GLOB FILES_WORK_SPACE ${WORK_SPACE}/*.cpp)
file(GLOB FILES_SRC ${SRC}/*.cpp)
file(GLOB FILES_MEIDA_DATA ${MEDIA_DATA}/*.cpp)
file(GLOB FILES_UTILS ${UTILS}/*.cpp)

add_executable(FFmpegCore
        ${FILES_WORK_SPACE}
        ${FILES_SRC}
        ${FILES_MEIDA_DATA}
        ${FILES_UTILS})