# Minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Project name and languages
project(exercice2 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Path to include directories
include_directories(src/include)

# Set the build type to Release
set(CMAKE_BUILD_TYPE Release)

# Path for output binaries
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/Release)

# Source files for the executable
set(SOURCES src/main.cpp src/Metrics.cpp)

# Add the executable
add_executable(exercice2 ${SOURCES})

# Enable verbose makefile output (optional)
set(CMAKE_VERBOSE_MAKEFILE OFF)

# Find and link TBB
find_package(TBB REQUIRED)
if(TBB_FOUND)
    target_link_libraries(exercice2 PRIVATE TBB::tbb)
endif()