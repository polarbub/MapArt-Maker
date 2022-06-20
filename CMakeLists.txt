cmake_minimum_required(VERSION 3.21)
project(MapArtMaker)

set(CMAKE_EXPORT_COMPILE_COMMANDS)

option(BUILD_GUI "Enable building the GUI" ON)
option(BUILD_PROCESSING "Enable building the Processing part" ON)

#set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_BUILD_TYPE "Release")

if(BUILD_GUI OR BUILD_PROCESSING)
    set(CMAKE_MAKE_PROGRAM="make -j9")

    if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        #Make my compiler stop wining about silly things
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    endif()

    #Include headers for PSL
    include_directories("${CMAKE_SOURCE_DIR}/src/libraries/PSL/include")
    #Include project headers
    include_directories("${CMAKE_SOURCE_DIR}/src/include")

    #Set C++ Standard
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    add_subdirectory(${CMAKE_SOURCE_DIR}/src/libraries/PSL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

if(BUILD_GUI)
    add_subdirectory(src/GUI)
endif()
if(BUILD_PROCESSING)
    add_subdirectory(src/Processing)
endif()