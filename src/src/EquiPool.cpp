/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include "QueueStrategy.h"
#include <EquiPool.h>

#include <list>

namespace thpl::equi {

class Pool::EquiQueue : public IPool::QueueStrategy {
public:
  bool isEmpty() const override { return this->queue.empty(); };

  std::function<void(void)> pop() override {
    std::function<void(void)> temp = std::move(this->queue.front());
    this->queue.pop_front();
    return temp;
  };

  void push(std::function<void(void)> task) {
    this->queue.emplace_back(std::move(task));
  };

private:
  std::list<std::function<void(void)>> queue;
};

Pool::Pool(const std::size_t &poolSize)
    : IPool(poolSize, std::make_unique<Pool::EquiQueue>()) {}

void Pool::push(std::function<void(void)> newTask) {
  {
    std::scoped_lock<std::mutex> lk(this->queueMtx);
    static_cast<EquiQueue *>(this->queue.get())->push(std::move(newTask));
  }
  ++this->remainingTasks;
}

} // namespace thpl::equi