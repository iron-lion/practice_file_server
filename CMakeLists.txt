project(NOOB)

cmake_minimum_required(VERSION 3.8)

# prohibit in-source-builds
IF (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(STATUS "In-source-builds are not allowed")
    message(STATUS "Clean your source directory (e.g. delete the CMakeCache.txt")
    message(FATAL_ERROR "Please create a separate build directory and call cmake")
ENDIF (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

# set the default build type to debug
IF(NOT CMAKE_BUILD_TYPE)
#    SET(CMAKE_BULD_TYPE Debug CACHE STRING
#        "Choose the type of build. options are: Debug Release MinSizeRel."
#        FORCE)
    message(STATUS "No build type selected, default to Debug")
    set(CMAKE_BUILD_TYPE "Debug")
    message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
ENDIF(NOT CMAKE_BUILD_TYPE)

set(PYTHON_INCLUDE_DIR /usr/include/python2.7)
set(PYTHON_LIBRARY /usr/lib/python2.7/config-x86_64-linux-gnu/libpython2.7.so)

find_package(Threads)
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
add_subdirectory(src)
