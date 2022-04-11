# Declare the minimum required CMake version
cmake_minimum_required(VERSION 2.8.12)

# Name the project
project(MapArtMakerCLI)

include_directories("${CMAKE_SOURCE_DIR}/src/Processing")

#Remove header from SRC_FILES regex:
#' src/[^\n ]*\.h'
# Define a variable containing a list of source files for the project
set(SRC_FILES
        Main.cpp Conversion.cpp ColorSpace.cpp Comparison.cpp miniz.c nbt/nbtDriver.cpp)

# Define the build target for the executable
add_executable(${PROJECT_NAME}
        ${SRC_FILES})

if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    #Make my compiler stop wining about silly things
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wno-comment")
endif()

# Link libraries to the executable
#ADD: Make libraries staticly linked
target_link_libraries(${PROJECT_NAME} PSL)