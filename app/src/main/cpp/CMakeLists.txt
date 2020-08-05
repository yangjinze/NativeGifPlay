# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

include_directories("${PROJECT_SOURCE_DIR}") #cmakelist所在路径

add_library(gif_hanndle SHARED gif_handle.cpp ./gif/dgif_lib.c ./gif/gifalloc.c)
#add_library(gif_hanndle SHARED gif_handle.cpp)

find_library(log-lib log )

target_link_libraries( gif_hanndle ${log-lib} jnigraphics)