This package is a C++ implementation of the [thread pool](https://en.wikipedia.org/wiki/Thread_pool) pattern.
Remember to leave a **star** if this package was useful.

There are essentially 3 ways to use this library:

 * **CppThreadPool::Fifo** implements a standard thread pool, where tasks are executed with a FIFO logic
 * **CppThreadPool::Lifo** implements a similar thread pool logics, where tasks are executed with a LIFO logic
 * **CppThreadPool::Prioritized** implements a prioritized thread pool, where the tasks externally feeded are internally ordered according to a prescribed priority level
 
All the functionalities are contained in [this](./src/CppThreadPool/CppThreadPool.hxx) single header file!
Using this package is straightforward. You just have to create the pool you want to use:
```cpp
#include <CppThreadPool/CppThreadPool.hxx>

const std::size_t pool_size = 4;
// build a new thread pool with the specified size
CppThreadPool::Fifo thread_pool(
    pool_size); // thread pool with a FIFO logic: you can also decide to use a
                // LIFO or a prioritized queue
```

... and push the tasks that you want to be parallely executed by the thread pool:
```cpp
void dummy_task() {
  // do something meaningfull
}

thread_pool.push(dummy_task);
```

Task can be passed also in the form of **lambda functions**:
```cpp
thread_pool.push([]() {
// do something meaningfull
});
```

You can create synchronization points by waiting for all tasks to be completed:
```cpp
// wait for the all pushed tasks to be completed
  thread_pool.wait();
```

Note that the d'tor of a thread pool will internally call wait().
Haven't yet left a **star**? Do it now :).

You can also push a bunch of tasks, but wait for the completion of a specific one (or a group of ones):
```cpp
// push and wait for completion of only the second task
thread_pool.push(dummy_task);
std::future<void> notification = thread_pool.push(dummy_task);
thread_pool.push(dummy_task);
notification.wait();
```

The future returned when pushing the task can be also used to catch the exception that might hev been thrown when the pool executed the task:
```cpp
// in case something went wrong calling get will let you catch the throwned
// exception
try {
  notification.get();
} catch (const std::exception &e) {
  // do smething with e
}
```

Check also the 3 examples provided in [samples](./samples) for more insights.

----------------------------------------------------------------------------------

**Compile**

Haven't yet left a **star**? Do it now :).

All functionalities are contained in [this](./src/CppThreadPool/CppThreadPool.hxx) single header file. 
Therefoer, integrating **CppThreadPool** is easy. In order to compile, you need to link to **Threads**. The **CMake** structure provided in this repo can automatically do this for you.
Indeed, you can fetch this package and link to the **CppThreadPool** library:
```cmake
include(FetchContent)
FetchContent_Declare(
cpp_thread_pool
GIT_REPOSITORY https://github.com/andreacasalino/Cpp-Thread-Pool
GIT_TAG        master
)
FetchContent_MakeAvailable(cpp_thread_pool)
```
Notice that **CppThreadPool** it's just an [**INTERFACE**](http://mariobadr.com/creating-a-header-only-library-with-cmake.html) library that expose [this](./src/CppThreadPool/CppThreadPool.hxx) header and links to **Threads**.
Linking to **CppThreadPool** you can then use all the functionalities:
```cmake
target_link_libraries(${TARGET_NAME}
   CppThreadPool
)

to compile and run the examples simply use [CMake](https://cmake.org). Let the CMake script detect your OS and link the correct thread library ;).

