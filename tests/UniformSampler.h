#pragma once

#include <CppThreadPool/CppThreadPool.hxx>

#include <random>

namespace CppThreadPool::test {
class UniformSampler {
public:
  UniformSampler();

  Priority sample() const {
    return static_cast<Priority>(this->distribution(this->generator));
  };

  void resetSeed(const std::size_t &newSeed) {
    this->generator.seed(static_cast<unsigned int>(newSeed));
  }

private:
  mutable std::mt19937 generator;
  mutable std::uniform_int_distribution<> distribution;
};

} // namespace CppThreadPool::test
