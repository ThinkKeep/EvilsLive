# Sets the minimum version of CMake required to build your native library.
# This ensures that a certain set of CMake features is available to
# your build.

cmake_minimum_required(VERSION 3.4.1)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add.library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

add_library( # Specifies the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             videolib/src/main/cpp/native-lib.cpp )

include_directories (videolib/src/main/cpp/include)

aux_source_directory(videolib/src/main/cpp/rtmp/ DIR_LIB_SRCS)

# link_directories ("../../../../src/main/cpp/lib")
add_library( # Specifies the name of the library.
             rtmp_push

             # Sets the library as a shared library.
             STATIC

             # Provides a relative path to your source file(s).
             ${DIR_LIB_SRCS})

find_library( # Defines the name of the path variable that stores the
              # location of the NDK library.
              log-lib

              # Specifies the name of the NDK library that
              # CMake needs to locate.
              log )
find_library(rtmp-lib NAMES rtmp
    HINTS videolib/src/main/cpp/lib
    CMAKE_FIND_ROOT_PATH_BOTH NO_DEFAULT_PATH)

find_library(h264enc-lib NAMES h264enc
    HINTS videolib/src/main/cpp/lib
    CMAKE_FIND_ROOT_PATH_BOTH NO_DEFAULT_PATH)

find_library(x264-lib NAMES x264
    HINTS videolib/src/main/cpp/lib
    CMAKE_FIND_ROOT_PATH_BOTH NO_DEFAULT_PATH)

find_library(clogger-lib NAMES clogger
    HINTS videolib/src/main/cpp/lib
    CMAKE_FIND_ROOT_PATH_BOTH NO_DEFAULT_PATH)

# MESSAGE( STATUS "CMAKE_BINARY_DIR:         " ${RTMP-LIBRARY} )

# Links your native library against one or more other native libraries.
target_link_libraries( # Specifies the target library.
                       native-lib

					   rtmp_push

					   ${rtmp-lib}
					   ${h264enc-lib}
					   ${x264-lib}
					   ${clogger-lib}

                       # Links the log library to the target library.
                       ${log-lib} )