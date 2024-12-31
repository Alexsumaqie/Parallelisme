cmake_minimum_required(VERSION 3.0.0)
project(exercice4 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

include_directories(src/include)

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

find_package(TBB REQUIRED)

add_executable(exercice4 src/pearson.cpp)

target_link_libraries(exercice4 TBB::tbb)