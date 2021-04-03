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
	add_library(${PROJECT_SHORTNAME} SHARED ${SOURCES})
else()
	add_library(${PROJECT_SHORTNAME} STATIC ${SOURCES})
endif()
add_library(thpl::${PROJECT_SHORTNAME} ALIAS ${PROJECT_SHORTNAME})

target_compile_features(${PROJECT_SHORTNAME}
    PUBLIC cxx_auto_type
    PRIVATE cxx_variadic_templates
)

target_include_directories(${PROJECT_SHORTNAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/header>
)

find_package(Threads)
target_link_libraries(${PROJECT_SHORTNAME}
    PRIVATE
	${CMAKE_THREAD_LIBS_INIT}      
)

