/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <chrono>
#include <iostream>

#include <EquiPool.h>
using namespace std;
using namespace thpl::equi;

void dummy_task() {
  // do something meaningfull
}

int main() {
  const std::size_t pool_size = 4;
  // build a new thread pool with the specified size
  Pool thread_pool(pool_size);

  // add tasks that will be parallely executed by the pool
  thread_pool.push(dummy_task);
  thread_pool.push([]() {
    // do something meaningfull
  });

  // wait for the all pushed tasks to be completed
  thread_pool.wait();

  // add more tasks
  thread_pool.push(dummy_task);
  thread_pool.push([]() {
    // do something meaningfull
  });

  // inside the thread_pool d'tor wait() will be called again
  return EXIT_SUCCESS;
}
