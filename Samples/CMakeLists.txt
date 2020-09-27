add_executable(
Sample01
./Main01.cpp
)
target_link_libraries( Sample01
	PUBLIC
		thpl::Thread-Pool
)

add_executable(
Sample02
./Main02.cpp
)
target_link_libraries( Sample02
	PUBLIC
		thpl::Thread-Pool
)

add_executable(
Sample03
./Main03.cpp
)
target_link_libraries( Sample03
	PUBLIC
		thpl::Thread-Pool
)

