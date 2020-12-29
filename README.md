This package is a C++ implementation of the [thread pool](https://en.wikipedia.org/wiki/Thread_pool) pattern.

There are essentially 2 ways to use this library:

 * use ./Lib/header/**EquiPool.h** to create and control a standard thread pool, where tasks are executed with a FIFO logic
 * use ./Lib/header/**PrioritizedPool.h** to create and control a prioritized thread pool, where tasks that are externally feeded, are ordered according to a priority level

Check the comments in the headers for more info.

Check the 3 examples provided in ./Samples to easily understand how to use the library.

----------------------------------------------------------------------------------

**Compile**

to compile and run the examples simply use [CMake](https://cmake.org). Let the CMake script detect your OS and link the correct thread library ;).

