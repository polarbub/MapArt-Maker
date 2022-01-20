cmake_minimum_required(VERSION 3.21)
project(MapArtMaker)

option(BUILD_GUI "Enable building the GUI" ON)
option(BUILD_PROCESSING "Enable building the Processing part" ON)

if(BUILD_GUI)
    add_subdirectory(src/GUI)
endif()
if(BUILD_PROCESSING)
    add_subdirectory(src/Processing)
endif()