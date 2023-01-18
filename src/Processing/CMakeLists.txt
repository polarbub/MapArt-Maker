# Declare the minimum required CMake version
# Name the project
project(MapArtMakerCLI)

#Remove header from SRC_FILES regex:
#' src/[^\n ]*\.h'
# Define the build target for the executable
add_executable(${PROJECT_NAME} Main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/Processing")

#Fix clang not building inline asm right
#if ( CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
#endif()

# Link libraries to the executable
target_link_libraries(${PROJECT_NAME} PSL)