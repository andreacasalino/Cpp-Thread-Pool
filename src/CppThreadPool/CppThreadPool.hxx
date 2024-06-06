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
#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <queue>
#include <stack>
#include <thread>

namespace CppThreadPool {
namespace detail {
struct Task {
  template <typename Pred>
  Task(Pred &&pred) : action{std::forward<Pred>(pred)} {}

  std::promise<void> notifier;
  std::function<void()> action;
};
using TaskPtr = std::unique_ptr<Task>;

template <typename Pred> TaskPtr make_task(Pred &&pred) {
  return std::make_unique<Task>(std::forward<Pred>(pred));
}

class FifoTasksContainer {
protected:
  FifoTasksContainer() = default;

  void push(TaskPtr task) { tasks.emplace_back(std::move(task)); }
  TaskPtr pop() {
    return nullptr;
    // TaskPtr result = std::move(tasks.front());
    // tasks.pop_front();
    // return result;
  }

  bool empty() const { return tasks.empty(); }

private:
  std::deque<TaskPtr> tasks;
};

class LifoTasksContainer {
protected:
  LifoTasksContainer() = default;

  void push(TaskPtr task) { tasks.emplace(std::move(task)); }
  TaskPtr pop() {
    TaskPtr result = std::move(tasks.top());
    tasks.pop();
    return result;
  }

  bool empty() const { return tasks.empty(); }

private:
  std::stack<TaskPtr> tasks;
};
} // namespace detail

using Priority = unsigned;

namespace detail {
class PrioritizedTasksContainer {
protected:
  PrioritizedTasksContainer() = default;

  struct Element {
    Priority priority;
    TaskPtr task;

    bool operator<(const Element &o) const { return priority < o.priority; }
  };

  void push(TaskPtr task, const Priority &p) {
    tasks.emplace(Element{p, std::move(task)});
  }
  TaskPtr pop() {
    auto &top = const_cast<TaskPtr &>(tasks.top().task);
    TaskPtr result = std::move(top);
    tasks.pop();
    return result;
  }

  bool empty() const { return tasks.empty(); }

private:
  std::priority_queue<Element> tasks;
};

struct SpinLockGuard {
  SpinLockGuard(std::atomic_bool &lock) : lock{lock} {
    while (true) {
      bool expected = true;
      if (lock.compare_exchange_strong(
              expected, false, std::memory_order::memory_order_acquire)) {
        break;
      }
    }
  }

  ~SpinLockGuard() {
    lock.store(true, std::memory_order::memory_order_release);
  }

private:
  std::atomic_bool &lock;
};
} // namespace detail

template <typename TaskContainerT>
class ThreadPool final : public TaskContainerT {
public:
  ThreadPool(const std::size_t &poolSize) {
    if (0 == poolSize) {
      throw std::runtime_error("Pool size should be at least 1");
    }
    for (std::size_t k = 0; k < poolSize; ++k) {
      workers.emplace_back([this]() { this->workerLoop(); });
    }
  }

  ThreadPool(const ThreadPool &) = delete;
  void operator=(const ThreadPool &) = delete;
  ThreadPool(ThreadPool &&) = delete;
  void operator=(ThreadPool &&) = delete;

  /**
   * \brief Destroyer. Threads inside the pool are killed even if the queue of
   * tasks to be done is not empty.
   */
  ~ThreadPool() {
    keep_run.store(false, std::memory_order::memory_order_acquire);
    for (auto &worker : workers) {
      worker.join();
    }
  }

  template <typename Pred, typename... Args>
  std::future<void> push(Pred &&action, Args &&...args) {
    auto task_ptr = detail::make_task(std::forward<Pred>(action));
    std::future<void> retVal = task_ptr->notifier.get_future();
    {
      detail::SpinLockGuard guard{lock};
      this->TaskContainerT::push(std::move(task_ptr),
                                 std::forward<Args>(args)...);
      tasks_numb.fetch_add(1, std::memory_order::memory_order_release);
    }
    return retVal;
  }

  /**
   * \brief Block the caller till all tasks pushed to the pool flushes all
   * pushed tasks.
   */
  void wait() {
    while (keep_run.load(std::memory_order_acquire) &&
           tasks_numb.load(std::memory_order_acquire) != 0) {
    }
  }

  /**
   * \brief Returns the size of the pool, i.e. the number of workers
   */
  std::size_t size() const { return this->workers.size(); };

private:
  void workerLoop() {
    while (keep_run.load(std::memory_order_acquire)) {
      if (tasks_numb.load(std::memory_order_acquire) == 0) {
        continue;
      }
      detail::TaskPtr task;
      {
        detail::SpinLockGuard guard{lock};
        if (this->TaskContainerT::empty()) {
          continue;
        }
        task = std::move(this->TaskContainerT::pop());
        tasks_numb.fetch_add(-1, std::memory_order::memory_order_release);
      }
      // auto &[notifier, action] = *task;
      // try {
      //   action();
      //   notifier.set_value();
      // } catch (...) {
      //   notifier.set_exception(std::current_exception());
      // }
    }
  }

  std::atomic_bool keep_run = true;
  std::atomic<std::size_t> tasks_numb = 0;
  std::atomic_bool lock = true;

  std::vector<std::thread> workers;
};

using Fifo = ThreadPool<detail::FifoTasksContainer>;

using Lifo = ThreadPool<detail::LifoTasksContainer>;

using Prioritized = ThreadPool<detail::PrioritizedTasksContainer>;
} // namespace CppThreadPool

#endif
