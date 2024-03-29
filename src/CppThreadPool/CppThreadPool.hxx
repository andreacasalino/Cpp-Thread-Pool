/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef __CPP_THREAD_POOL__
#define __CPP_THREAD_POOL__

#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <thread>

namespace CppThreadPool {
namespace detail {
struct Task {
  std::promise<void> notifier;
  std::function<void()> action;
};
using TaskPtr = std::unique_ptr<Task>;

std::pair<std::future<void>, TaskPtr>
make_task(const std::function<void()> &action) {
  TaskPtr result = std::make_unique<Task>();
  result->action = action;
  return std::make_pair(result->notifier.get_future(), std::move(result));
}

class FifoTasksContainer {
protected:
  FifoTasksContainer() = default;

  void push(TaskPtr task) { tasks.emplace_back(std::move(task)); }
  TaskPtr pop() {
    TaskPtr result = std::move(tasks.front());
    tasks.pop_front();
    return result;
  }

  bool empty() const { return tasks.empty(); }

private:
  std::list<TaskPtr> tasks;
};

class LifoTasksContainer {
protected:
  LifoTasksContainer() = default;

  void push(TaskPtr task) { tasks.emplace_back(std::move(task)); }
  TaskPtr pop() {
    TaskPtr result = std::move(tasks.back());
    tasks.pop_back();
    return result;
  }

  bool empty() const { return tasks.empty(); }

private:
  std::list<TaskPtr> tasks;
};
} // namespace detail

using Priority = unsigned;

namespace detail {
class PrioritizedTasksContainer {
protected:
  PrioritizedTasksContainer() = default;

  void push(TaskPtr task, const Priority &p) {
    tasks.emplace(p, std::move(task));
  }
  TaskPtr pop() {
    auto top = tasks.begin();
    TaskPtr result = std::move(top->second);
    tasks.erase(top);
    return result;
  }

  bool empty() const { return tasks.empty(); }

private:
  std::multimap<Priority, TaskPtr, std::greater<Priority>> tasks;
};
} // namespace detail

template <typename TaskContainerT> class ThreadPool : public TaskContainerT {
public:
  ThreadPool(const std::size_t &poolSize) {
    if (0 == poolSize) {
      throw std::runtime_error("Pool size should be at least 1");
    }

    struct SpawnInfo {
      std::atomic_bool waiting_all_spawned = true;
      std::mutex spawned_mtx;
      std::size_t spawned = 0;
    } spawn_info;
    // spawn all the threads in the pool
    workers.reserve(poolSize);
    for (std::size_t k = 0; k < poolSize; ++k) {
      workers.emplace_back(
          std::make_unique<std::thread>([&spawn_info, &poolSize, this]() {
            {
              std::scoped_lock lock(spawn_info.spawned_mtx);
              ++spawn_info.spawned;
              spawn_info.waiting_all_spawned = spawn_info.spawned != poolSize;
            }
            while (this->poolLife) {
              if (no_new_tasks) {
                continue;
              }
              detail::TaskPtr task;
              {
                std::scoped_lock lock(tasks_mtx);
                const bool is_now_empty = this->TaskContainerT::empty();
                no_new_tasks = is_now_empty;
                if (is_now_empty) {
                  continue;
                }
                task = this->TaskContainerT::pop();
              }
              try {
                task->action();
                task->notifier.set_value();
              } catch (...) {
                task->notifier.set_exception(std::current_exception());
              }
              {
                std::scoped_lock counter_lock(counter.mtx);
                --counter.tasks_to_complete;
                counter.something_to_complete = counter.tasks_to_complete != 0;
              }
            }
          }));
    }

    // make sure all the threads were spawned before returning
    while (spawn_info.waiting_all_spawned) {
    }
  }

  ThreadPool(const ThreadPool &) = delete;
  void operator=(const ThreadPool &) = delete;

  /**
   * \brief Destroyer. Threads inside the pool are killed even if the queue of
   * tasks to be done is not empty.
   */
  virtual ~ThreadPool() {
    poolLife = false;
    for (auto &worker : workers) {
      if (worker->joinable()) {
        worker->join();
      }
    }
  }

  template <typename... Args>
  std::future<void> push(const std::function<void()> &action, Args... args) {
    {
      std::scoped_lock counter_lock(counter.mtx);
      ++counter.tasks_to_complete;
      counter.something_to_complete = true;
    }
    auto new_task = detail::make_task(action);
    std::scoped_lock lock(tasks_mtx);
    this->TaskContainerT::push(std::move(new_task.second),
                               std::forward<Args>(args)...);
    no_new_tasks = false;
    return std::move(new_task.first);
  }

  /**
   * \brief Blocking wait for all the previously pushed tasks to be completed.
   * Threads are not destroyed after, and new tasks can be passed to pool.
   */
  void wait() {
    while (counter.something_to_complete) {
    }
  }

  /**
   * \brief Returns the size of the pool
   */
  std::size_t size() const { return this->workers.size(); };

private:
  std::atomic<bool> poolLife = true;
  std::vector<std::unique_ptr<std::thread>> workers;

  std::mutex tasks_mtx;
  std::atomic_bool no_new_tasks = true;
  struct TaskToCompleteCounter {
    std::mutex mtx;
    std::size_t tasks_to_complete = 0;
    std::atomic_bool something_to_complete = false;
  };
  TaskToCompleteCounter counter;
};

using Fifo = ThreadPool<detail::FifoTasksContainer>;
using Lifo = ThreadPool<detail::LifoTasksContainer>;
using Prioritized = ThreadPool<detail::PrioritizedTasksContainer>;
} // namespace CppThreadPool

#endif
