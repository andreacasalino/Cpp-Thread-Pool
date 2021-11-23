cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

option(LIB_OPT "Compile shared libraries (ON) or static (OFF)" OFF)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

project(Thread-Pool-Core)
add_subdirectory(Lib)

option(BUILD_Thread_Pool_SAMPLES "" ON)
if(BUILD_Thread_Pool_SAMPLES)
    add_subdirectory(Samples)
endif()
