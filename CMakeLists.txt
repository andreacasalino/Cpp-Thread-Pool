cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

add_subdirectory(src)

option(BUILD_Thread_Pool_SAMPLES "" ON)
if(BUILD_Thread_Pool_SAMPLES)
   add_subdirectory(samples)
endif()

option(BUILD_Thread_Pool_TESTS "" OFF)
if(BUILD_Thread_Pool_TESTS)
   add_subdirectory(tests)
endif()
