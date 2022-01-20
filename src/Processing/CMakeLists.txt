# Declare the minimum required CMake version
cmake_minimum_required(VERSION 2.8.12)

# Name the project
project(MapArtMakerCLI)
include_directories("${CMAKE_SOURCE_DIR}")

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE Debug)
#Remove header from SRC_FILES regex:
#' src/[^\n ]*\.h'
# Define a variable containing a list of source files for the project
set(SRC_FILES
        Main.cpp Conversion.cpp ColorSpace.cpp Comparison.cpp miniz.c)

# Define the build target for the executable
add_executable(${PROJECT_NAME}
        ${SRC_FILES})