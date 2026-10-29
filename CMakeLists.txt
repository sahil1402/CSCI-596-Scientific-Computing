cmake_minimum_required(VERSION 3.16)
project(HarborTune VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB HARBORTUNE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(harbortune ${HARBORTUNE_SOURCES})
target_include_directories(harbortune PRIVATE include)
target_compile_options(harbortune PRIVATE -Wall -Wextra -Wpedantic)

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(harbortune PRIVATE OpenMP::OpenMP_CXX)
else()
    target_compile_definitions(harbortune PRIVATE HARBORTUNE_NO_OPENMP)
    message(WARNING "OpenMP was not found; building the sequential fallback")
endif()

