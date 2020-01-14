cmake_minimum_required(VERSION 3.10)
project(FFmpegCore)

set(CMAKE_CXX_STANDARD 11)

set(WORK_SPACE ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC ${WORK_SPACE}/src)
set(MEDIA_DATA ${SRC}/media_data)
set(UTILS ${SRC}/utils)
set(FFMPEG_EXAMPLE ${SRC}/ffmpeg_example)

include_directories(${WORK_SPACE})
include_directories(${SRC})
include_directories(${MEDIA_DATA})
include_directories(${UTILS})
include_directories(${FFMPEG_EXAMPLE})

file(GLOB FILES_WORK_SPACE ${WORK_SPACE}/*.cpp)
file(GLOB FILES_SRC ${SRC}/*.cpp)
file(GLOB FILES_MEIDA_DATA ${MEDIA_DATA}/*.cpp)
file(GLOB FILES_UTILS ${UTILS}/*.cpp)
file(GLOB FILES_FFMPEG_EXAMPLE ${FFMPEG_EXAMPLE}/*.cpp)

add_executable(FFmpegCore
        ${FILES_WORK_SPACE}
        ${FILES_SRC}
        ${FILES_MEIDA_DATA}
        ${FILES_UTILS}
        ${FILES_FFMPEG_EXAMPLE})

#MAC下开发FFMPEG的cmake: https://blog.csdn.net/zzcchunter/article/details/60959963
INCLUDE_DIRECTORIES(/usr/local/Cellar/ffmpeg/4.1.3/include)
INCLUDE_DIRECTORIES(/usr/local/Cellar/ffmpeg/4.1.3/lib)
INCLUDE_DIRECTORIES(/usr/local/Cellar/libass/0.14.0_1/include)
INCLUDE_DIRECTORIES(/usr/local/Cellar/libass/0.14.0_1/lib)
INCLUDE_DIRECTORIES(/usr/X11/lib)
INCLUDE_DIRECTORIES(/usr/lib)
FIND_LIBRARY(LIBAVCODEC libavcodec.dylib)
FIND_LIBRARY(LIBAVDEVICE libavdevice.dylib)
FIND_LIBRARY(LIBAVFILTER libavfilter.dylib)
FIND_LIBRARY(LIBAVFORMAT libavformat.dylib)
FIND_LIBRARY(LIBAVUTIL libavutil.dylib)
FIND_LIBRARY(LIBPOSTPROC libpostproc.dylib)
FIND_LIBRARY(LIBSWRESAMPLE libswresample.dylib)
FIND_LIBRARY(LIBSWSCALE libswscale.dylib)
FIND_LIBRARY(LIBPOSTPROC libpostproc.dylib)
FIND_LIBRARY(LIBICONV libiconv.dylib)
FIND_LIBRARY(LIBAVRESAMPLE libavresample.dylib)
FIND_LIBRARY(LIBFREETYPE libfreetype.dylib)
FIND_LIBRARY(LIBASS libass.dylib)


MARK_AS_ADVANCED(LIBAVCODEC LIBAVDEVICE LIBAVFILTER LIBMP3LAME LIBAVUTIL LIBAVRESAMPLE LIBSWSCALE LIBAVFORMAT LIBPOSTPROC LIBSWRESAMPLE LIBICONV LIBFREETYPE LIBASS)
set(extra_libs ${LIBAVCODEC} ${LIBAVDEVICE} ${LIBAVFILTER} ${LIBMP3LAME} ${LIBAVUTIL} ${LIBAVRESAMPLE} ${LIBSWSCALE} ${LIBAVFORMAT} ${LIBPOSTPROC} ${LIBSWRESAMPLE} ${LIBICONV} ${LIBFREETYPE} ${LIBASS})

target_link_libraries(FFmpegCore ${extra_libs})