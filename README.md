This package is a C++ implementation of the [thread pool](https://en.wikipedia.org/wiki/Thread_pool) pattern.
Remember to leave a **star** if this package was useful.

There are essentially 2 ways to use this library:

 * use ./Lib/header/**EquiPool.h** to create and control a standard thread pool, where tasks are executed with a FIFO logic
 * use ./Lib/header/**PrioritizedPool.h** to create and control a prioritized thread pool, where tasks that are externally feeded, are ordered according to a priority level

Using this package is straightforward. You just have to build the thread pool you want to use:
```cpp
#include <EquiPool.h>
using namespace std;
using namespace thpl::equi;

const std::size_t pool_size = 4;
// build a new thread pool with the specified size
Pool thread_pool(pool_size);
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
Check also the 3 examples provided in ./Samples for more insights.
Haven't yet left a **star**? Do it now :).

----------------------------------------------------------------------------------

**Compile**

to compile and run the examples simply use [CMake](https://cmake.org). Let the CMake script detect your OS and link the correct thread library ;).
