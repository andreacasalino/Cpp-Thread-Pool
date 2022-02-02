/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include "QueueStrategy.h"
#include <PrioritizedPool.h>
#include <queue>
#include <utility>

namespace thpl::prty {

class Pool::PriorityQueue : public IPool::QueueStrategy {
public:
  PriorityQueue();

  bool isEmpty() const override { return this->queue.empty(); };

  std::function<void(void)> pop() override {
    std::function<void(void)> temp = std::move(this->queue.top().second);
    this->queue.pop();
    return temp;
  };

  void push(std::function<void(void)> task, const unsigned int &pr) {
    this->queue.emplace(std::make_pair(pr, std::move(task)));
  };

private:
  using Task = std::pair<unsigned int, std::function<void(void)>>;
  std::priority_queue<Task, std::vector<Task>,
                      std::function<bool(const Task &, const Task &)>>
      queue;
};

Pool::PriorityQueue::PriorityQueue()
    : queue([](const Task &a, const Task &b) { return a.first < b.first; }){};

Pool::Pool(const std::size_t &poolSize)
    : IPool(poolSize, std::make_unique<PriorityQueue>()) {}

void Pool::push(std::function<void(void)> newTask,
                const unsigned int priority) {
  {
    std::scoped_lock<std::mutex> lk(this->queueMtx);
    static_cast<PriorityQueue *>(this->queue.get())
        ->push(std::move(newTask), priority);
  }
  ++this->remainingTasks;
}

} // namespace thpl::prty