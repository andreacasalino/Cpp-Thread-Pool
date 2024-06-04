#include "UniformSampler.h"

namespace CppThreadPool::test {
UniformSampler::UniformSampler()
    : generator(std::random_device{}()), distribution(1, 20) {
  resetSeed(2);
}
} // namespace CppThreadPool::test
