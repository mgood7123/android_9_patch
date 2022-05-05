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
    NinePatch.cpp
    JenkinsHash.cpp
    Unicode.cpp
)





# googletest

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(googletest)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("googletest/include")
endif()

add_executable(tests
    9patch_tests.cpp
)

target_link_libraries(tests android_9_patch gtest_main)
