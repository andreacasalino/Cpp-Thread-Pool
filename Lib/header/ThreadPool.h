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
#include <mutex>
#include <thread>

namespace thpl {

/**
 * \brief Thread pool interface.
 * Tasks can be added to the pool, by calling push in deriving objects
 */
class IPool {
public:
  /**
   * \brief Destroyer. Threads inside the pool are killed even if the queue of
   * tasks to be done is not empty.
   */
  virtual ~IPool();
  /**
   * \brief Blocking call to wait for all the tasks to be finished.
   * Threads are not destroyed after, and new tasks can be passed to pool.
   */
  void wait();

  IPool(const IPool &) = delete;
  void operator=(const IPool &) = delete;

  /**
   * \brief Returns the size of the pool
   */
  std::size_t size() const { return this->pool.size(); };

protected:
  class QueueStrategy;
  std::unique_ptr<QueueStrategy> queue;
  std::mutex queueMtx;

  IPool(const std::size_t &poolSize, std::unique_ptr<QueueStrategy> queue);

  std::atomic<std::size_t> remainingTasks;

private:
  std::atomic<bool> poolLife;
  std::vector<std::thread> pool;
};

} // namespace thpl
