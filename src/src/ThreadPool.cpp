/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include "QueueStrategy.h"

namespace thpl {

IPool::IPool(const std::size_t &poolSize, std::unique_ptr<QueueStrategy> queue)
    : queue(std::move(queue)), poolLife(true), remainingTasks(0) {
  if (poolSize == 0) {
    throw std::runtime_error("invalid pool size");
  }

  std::atomic<std::size_t> runningThreads = 0;

  // spawn all the threads in the pool
  this->pool.reserve(poolSize);
  for (std::size_t k = 0; k < poolSize; ++k) {
    this->pool.emplace_back([this, &runningThreads]() {
      ++runningThreads;
      bool isEmpty;
      while (this->poolLife) {
        std::function<void(void)> task;
        {
          std::scoped_lock<std::mutex> lk(this->queueMtx);
          isEmpty = this->queue->isEmpty();
          if (!isEmpty)
            task = this->queue->pop();
        }
        if (!isEmpty) {
          task();
          --this->remainingTasks;
        }
      }
    });
  }

  // make sure all the threads were spawned before returning
  while (runningThreads < poolSize) {
    std::this_thread::sleep_for(std::chrono::milliseconds{1});
  }
}

void IPool::wait() {
  while (true) {
    if (this->remainingTasks == 0)
      return;
  }
}

IPool::~IPool() {
  this->poolLife = false;
  for (auto &th : this->pool) {
    if (th.joinable()) {
      th.join();
    }
  }
}

} // namespace thpl