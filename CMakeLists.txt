cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

option(LIB_OPT "Compile shared libraries (ON) or static (OFF)" OFF)
set(CMAKE_CXX_STANDARD 17)

project(Thread-Pool-Core)
add_subdirectory(Lib)

add_subdirectory(Samples)
