/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef __CPP_THREAD_POOL__
#define __CPP_THREAD_POOL__

#include <atomic>
#include <functional>
#include <future>
#include <list>
#include <memory>
#include <queue>
#include <thread>

namespace CppThreadPool {
namespace detail {
struct Task {
  std::promise<void> notifier;
  std::function<void()> action;
};
using TaskPtr = std::unique_ptr<Task>;

TaskPtr make_task(const std::function<void()> &action, std::future<void> &out) {
  TaskPtr result = std::make_unique<Task>();
  result->action = action;
  out = result->notifier.get_future();
  return result;
}

class TasksQueue {
public:
  virtual ~TasksQueue() = default;

protected:
  TasksQueue() = default;

  std::atomic_bool empty = true;
  std::mutex tasks_mtx;
};
} // namespace detail

class FifoQueue : public detail::TasksQueue {
public:
  FifoQueue() = default;

protected:
  std::future<void> push_(const std::function<void()> &action) {
    std::scoped_lock lock(tasks_mtx);
    std::future<void> result;
    auto new_task = detail::make_task(action, result);
    tasks.emplace_back(std::move(new_task));
    empty = false;
    return result;
  }

  detail::TaskPtr pop_() {
    if (empty) {
      return nullptr;
    }
    std::scoped_lock lock(tasks_mtx);
    auto result = std::move(tasks.front());
    tasks.pop_front();
    empty = tasks.empty();
    return std::move(result);
  }

private:
  std::list<detail::TaskPtr> tasks;
};

class PriorityQueue : public detail::TasksQueue {
public:
  using Priority = unsigned;

  PriorityQueue() = default;

protected:
  std::future<void> push_(const std::function<void()> &action,
                          const Priority &priority) {
    std::scoped_lock lock(tasks_mtx);
    std::future<void> result;
    auto new_task = detail::make_task(action, result);
    tasks.emplace(priority, std::move(new_task));
    empty = false;
    return result;
  }

  detail::TaskPtr pop_() {
    if (empty) {
      return nullptr;
    }
    std::scoped_lock lock(tasks_mtx);
    auto result = std::move(tasks.top());
    tasks.pop();
    empty = tasks.empty();
    return std::move(result);
  }

private:
  std::priority_queue<Priority, detail::TaskPtr> tasks;
};

template <typename TasksQueueT> class ThreadPool : public TasksQueueT {
public:
  ThreadPool(const std::size_t &poolSize) {
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

      WorkerPtr worker = std::make_unique<Worker>(*this, std::thread{[this]() {
        while (this->poolLife) {
          auto task = this->pop_();
          if (nullptr == task) {
            continue;
          }
          --this->still_to_complete_tasks;
          try {
            task->action();
          } catch (...) {
            task->notifier.set_exception(std::current_exception());
          }
          task->notifier.set_value();
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

  ThreadPool(const ThreadPool &) = delete;
  void operator=(const ThreadPool &) = delete;

  /**
   * \brief Destroyer. Threads inside the pool are killed even if the queue of
   * tasks to be done is not empty.
   */
  ~ThreadPool() {
    poolLife = false;
    for (auto &worker : workers) {
      worker->loop.join();
      worker.reset();
    }
  }

  template <typename... Args> std::future<void> push(Args... args) {
    ++still_to_complete_tasks;
    return this->TasksQueueT::push_(args...);
  }

  /**
   * \brief Blocking wait for all the previously pushed tasks to be completed.
   * Threads are not destroyed after, and new tasks can be passed to pool.
   */
  void wait(const std::chrono::nanoseconds &polling_time =
                std::chrono::milliseconds{1}) {
    while (still_to_complete_tasks != 0) {
      std::this_thread::sleep_for(polling_time);
    }
  }

  /**
   * \brief Returns the size of the pool
   */
  std::size_t size() const { return this->workers.size(); };

private:
  std::atomic<bool> poolLife = true;
  std::atomic<std::size_t> still_to_complete_tasks = 0;

  struct Worker {
    ThreadPool &container;
    std::thread loop;
  };
  using WorkerPtr = std::unique_ptr<Worker>;
  std::vector<WorkerPtr> workers;
};

using ThreadPoolFifo = ThreadPool<FifoQueue>;
using ThreadPoolWithPriority = ThreadPool<PriorityQueue>;
} // namespace CppThreadPool

#endif
