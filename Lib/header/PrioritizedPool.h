/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <ThreadPool.h>

namespace thpl::prty {

/**
 * \brief Tasks are ordered in a priority queue
 */
class Pool : public IPool {
public:
  /**
   * \brief Constructor
   * @param[in] poolSize the number of threads to spawn that will be part of the
   * pool
   */
  Pool(const std::size_t &poolSize);
  /**
   * @param[in] newTask the newer task to add to the priority queue
   * @param[in] priority the priority of the task to insert
   */
  void push(std::function<void(void)> newTask, const unsigned int priority = 0);

private:
  class PriorityQueue;
  PriorityQueue *Q;
};

} // namespace thpl::prty
