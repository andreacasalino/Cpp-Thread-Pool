project(CppThreadPool-Tests)

include(FetchContent)
FetchContent_Declare(
	catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        4ff8b27bb6bed4b8b309e56cd269b4f1fbc24e89
)
FetchContent_MakeAvailable(catch2)

add_executable(Tests Tests.cpp)
target_link_libraries(Tests PUBLIC 
    CppThreadPool
    Catch2::Catch2
    Catch2::Catch2WithMain
)
install(TARGETS Tests)
