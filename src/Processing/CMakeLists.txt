# Declare the minimum required CMake version
cmake_minimum_required(VERSION 2.8.12)

# Name the project
project(MapArtMakerCLI)

include_directories("${CMAKE_SOURCE_DIR}/src/Processing")

#Remove header from SRC_FILES regex:
#' src/[^\n ]*\.h'
# Define a variable containing a list of source files for the project
set(SRC_FILES
        Main.cpp)

# Define the build target for the executable
add_executable(${PROJECT_NAME}
        ${SRC_FILES})

#Fix clang not building inline asm right
#if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
#endif()

# Link libraries to the executable
target_link_libraries(${PROJECT_NAME} PSL)