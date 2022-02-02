set(PROJECT_SHORTNAME "Thread-Pool")

list(APPEND SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/header/EquiPool.h
	${CMAKE_CURRENT_SOURCE_DIR}/header/PrioritizedPool.h
	${CMAKE_CURRENT_SOURCE_DIR}/header/ThreadPool.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/EquiPool.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/PrioritizedPool.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ThreadPool.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/QueueStrategy.h
)

if(LIB_OPT)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
	add_library(${PROJECT_SHORTNAME} SHARED ${SOURCES})
else()
	add_library(${PROJECT_SHORTNAME} STATIC ${SOURCES})
endif()

target_include_directories(${PROJECT_SHORTNAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/header
)

find_package(Threads)
target_link_libraries(${PROJECT_SHORTNAME}
    PRIVATE
	${CMAKE_THREAD_LIBS_INIT}      
)

install(TARGETS ${PROJECT_SHORTNAME})
install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/header/ DESTINATION include/${PROJECT_SHORTNAME} FILES_MATCHING PATTERN "*.h*")

