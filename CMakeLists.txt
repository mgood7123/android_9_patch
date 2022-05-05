cmake_minimum_required(VERSION 3.10.2)
project(android_9_patch)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(android_9_patch SHARED
    9patch.cpp
    Errors.cpp
    FileMap.cpp
    map_ptr.cpp
    NinePatchBindings.cpp
)
