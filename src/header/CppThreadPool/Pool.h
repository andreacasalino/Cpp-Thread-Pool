/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

namespace ThreadPool {
class TasksQueue;

class Pool {
public:
  /**
   * \brief Destroyer. Threads inside the pool are killed even if the queue of
   * tasks to be done is not empty.
   */
  virtual ~Pool();

  /**
   * \brief Blocking wait for all the previously pushed tasks to be completed.
   * Threads are not destroyed after, and new tasks can be passed to pool.
   */
  void wait();

  Pool(const Pool &) = delete;
  void operator=(const Pool &) = delete;

  /**
   * \brief Returns the size of the pool
   */
  std::size_t size() const { return this->workers.size(); };

protected:
  Pool(const std::size_t &poolSize, std::unique_ptr<TasksQueue> queue);

  TasksQueue &getQueue();
  template <typename T> T &getQueueAs() { return static_cast<T &>(getQueue()); }

private:
  struct PoolContext {
    std::atomic<bool> poolLife = true;
    std::unique_ptr<TasksQueue> queue;
  };
  PoolContext context;

  struct Worker {
    std::atomic_bool processing;
    PoolContext &context;
    std::thread loop;
  };
  using WorkerPtr = std::unique_ptr<Worker>;
  std::vector<WorkerPtr> workers;
};

} // namespace ThreadPool
