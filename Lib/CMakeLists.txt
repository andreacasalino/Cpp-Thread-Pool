set(PROJECT_SHORTNAME "Thread-Pool")
project(${PROJECT_SHORTNAME} VERSION ${VERSION} LANGUAGES CXX)
string(REPLACE - _ COMPONENT_NAME ${PROJECT_NAME})

CollectSourceFiles(${CMAKE_CURRENT_SOURCE_DIR} SOURCES)

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

if(LIB_OPT)
	add_library(${PROJECT_NAME} SHARED ${SOURCES})
else()
	add_library(${PROJECT_NAME} STATIC ${SOURCES})
endif()
add_library(thpl::${PROJECT_SHORTNAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME}
    PUBLIC cxx_auto_type
    PRIVATE cxx_variadic_templates
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/header>
        $<INSTALL_INTERFACE:header>
)

find_package(Threads)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
	${CMAKE_THREAD_LIBS_INIT}      
)

