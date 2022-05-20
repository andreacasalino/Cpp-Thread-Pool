/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include "TasksQueue.h"
#include <CppThreadPool/Pool.h>

#include <stdexcept>

namespace ThreadPool {
Pool::Pool(const std::size_t &poolSize, std::unique_ptr<TasksQueue> queue) {
  if (poolSize == 0) {
    throw std::runtime_error("invalid pool size");
  }

  std::vector<std::atomic_bool> spawned;
  spawned.resize(poolSize);
  // spawn all the threads in the pool
  workers.reserve(poolSize);
  for (std::size_t k = 0; k < poolSize; ++k) {
    auto &spawned_flag = spawned[k];
    spawned_flag = false;

    WorkerPtr worker = std::make_unique<Worker>(
        false, this->context, std::thread{[this]() {
          while (this->context.poolLife) {
            auto task = this->context.queue->pop();
            if (nullptr == task) {
              continue;
            }
            try {
              this->processing = true; // TODO maybe not correct for wait()
              task->action();
            } catch (...) {
              task->notifier.set_exception(std::current_exception());
            }
            task->notifier.set_value();
            this->processing = false;
          }
        }});
    workers.emplace_back(std::move(worker));
  }

  // make sure all the threads were spawned before returning
  for (const auto &flag : spawned) {
    while (!flag) {
    }
  }
}

TasksQueue &Pool::getQueue() { return *context.queue; }

void Pool::wait() {
  while (!getQueue().isEmpty()) {
  }
  for (const auto &worker : workers) {
    while (worker->processing) {
    }
  }
}

Pool::~Pool() {
  context.poolLife = false;
  for (auto &worker : workers) {
    worker->loop.join();
    worker.reset();
  }
}

} // namespace ThreadPool