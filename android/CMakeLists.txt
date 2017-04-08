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

add_library( # Specifies the name of the library.
             rtmp_push

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             videolib/src/main/cpp/rtmp/librtmppublish.cpp videolib/src/main/cpp/rtmp/rtmppublish.cpp videolib/src/main/cpp/rtmp/evils_live_api.cpp )

find_library( # Defines the name of the path variable that stores the
              # location of the NDK library.
              log-lib

              # Specifies the name of the NDK library that
              # CMake needs to locate.
              log )

# Links your native library against one or more other native libraries.
target_link_libraries( # Specifies the target library.
                       native-lib

					   rtmp_push
					   
					   videolib/src/main/cpp/lib/librtmp.a
					   videolib/src/main/cpp/lib/libh264enc.a
					   videolib/src/main/cpp/lib/libx264.a
					   videolib/src/main/cpp/lib/libclogger.a

                       # Links the log library to the target library.
                       ${log-lib} )