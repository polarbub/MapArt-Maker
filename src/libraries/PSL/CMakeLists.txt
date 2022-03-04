# Declare the minimum required CMake version
cmake_minimum_required(VERSION 2.8.12)

#Enable versioning
cmake_policy(SET CMP0048 NEW)

# set library metadata
project(PSL VERSION 0.0.1 DESCRIPTION "PSL!" LANGUAGES CXX)
set(CMAKE_PROJECT_VERSION_MAJOR 0)
set(CMAKE_PROJECT_VERSION_MINOR 0)
set(CMAKE_PROJECT_VERSION_PATCH 1)

#Remove header from SRC_FILES regex:
#' src/[^\n ]*\.h'
# Define a variable containing a list of source files for the project
set(SRC_FILES
        src/psl.cpp)

# Define the build target for the library
#add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

#Library options
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION 1)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER include/psl/psl.h)
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#target is used to only effect this library.
target_include_directories(${PROJECT_NAME} PRIVATE include/psl)