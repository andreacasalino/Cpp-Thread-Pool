project(Thread-Pool-Samples)

function(MAKE_SAMPLE NAME_SAMPLE)
    add_executable(${NAME_SAMPLE} ${CMAKE_CURRENT_SOURCE_DIR}/${NAME_SAMPLE}.cpp)
    target_link_libraries(${NAME_SAMPLE} PUBLIC Thread-Pool)	
		
	set_target_properties(${NAME_SAMPLE} PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;${CMAKE_INSTALL_PREFIX}/bin/")
	
	install(TARGETS ${NAME_SAMPLE})	
endfunction()

MAKE_SAMPLE(Sample01)
MAKE_SAMPLE(Sample02)
MAKE_SAMPLE(Sample03)
MAKE_SAMPLE(ReadME)
