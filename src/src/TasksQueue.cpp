/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include "TasksQueue.h"

namespace ThreadPool {
TaskPtr TasksQueue::pop() {
  if (empty) {
    return nullptr;
  }
  bool left_empty = false;
  auto result = this->pop_(left_empty);
  empty = left_empty;
  return std::move(result);
}

TaskPtr make_task(const std::function<void()> &action, std::future<void> &out) {
  TaskPtr result = std::make_unique<Task>();
  result->action = action;
  out = result->notifier.get_future();
  return result;
}
} // namespace ThreadPool
