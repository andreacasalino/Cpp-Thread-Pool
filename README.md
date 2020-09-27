You have essentiallz 2 options to use this library:
	1) use ./Lib/header/EquiPool.h to create and control a standard thread pool, where tasks are executed with a FIFO logic
	1) use ./Lib/header/PrioritizedPool.h to create and control a prioritized thread pool, where tasks that are externally feeded, are ordered according to a priority level

Check the comments in the headers for more info.

Check the 3 examples provided in ./Samples to easily understand how to use the library.

----------------------------------------------------------------------------------

to compile and run the examples simply use CMake. Let the CMake script detect your OS and link the correct thread library ;).

