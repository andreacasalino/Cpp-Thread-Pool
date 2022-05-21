project(CppThreadPool)
set(SHORTNAME "CppThreadPool")

add_library(${SHORTNAME} INTERFACE)

target_include_directories(${SHORTNAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Threads)
target_link_libraries(${SHORTNAME} INTERFACE ${CMAKE_THREAD_LIBS_INIT})
