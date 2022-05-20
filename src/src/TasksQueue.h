/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <CppThreadPool/Pool.h>
#include <atomic>
#include <future>

namespace ThreadPool {
struct Task {
  std::promise<void> notifier;
  std::function<void()> action;
};
using TaskPtr = std::unique_ptr<Task>;

class TasksQueue {
public:
  virtual ~TasksQueue() = default;

  bool isEmpty() const { return empty; };

  TaskPtr pop();

protected:
  TasksQueue() = default;

  virtual TaskPtr pop_(bool &left_empty) = 0;

private:
  std::atomic_bool empty = true;
};

TaskPtr make_task(const std::function<void()> &action, std::future<void> &out);
} // namespace ThreadPool
