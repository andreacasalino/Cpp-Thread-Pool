/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/
#pragma once

#include <ThreadPool.h>

namespace thpl {

class IPool::QueueStrategy {
public:
  virtual ~QueueStrategy() = default;

  virtual bool isEmpty() const = 0;

  virtual std::function<void(void)> pop() = 0;
};

} // namespace thpl
