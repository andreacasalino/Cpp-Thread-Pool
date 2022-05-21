#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <CppThreadPool/CppThreadPool.hxx>

#include <random>
#include <stdexcept>

namespace CppThreadPool {
class UniformSampler {
public:
  UniformSampler() : generator(std::random_device{}()), distribution(1, 20) {
    resetSeed(2);
  }

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

class PoolWrapper {
public:
  PoolWrapper(const std::size_t size, const bool fifo_or_prioritized) {
    if (fifo_or_prioritized) {
      as_fifo = std::make_unique<ThreadPoolFifo>(size);
    } else {
      as_prioritized = std::make_unique<ThreadPoolWithPriority>(size);
    }
  }

  std::future<void> push(const std::function<void()> &action) {
    if (nullptr == as_fifo) {
      Priority p = priority_sampler.sample();
      return as_prioritized->push(action, p);
    }
    return as_fifo->push(action);
  }

  void wait() {
    if (nullptr == as_fifo) {
      as_prioritized->wait();
    } else {
      as_fifo->wait();
    }
  }

  std::size_t size() const {
    if (nullptr == as_fifo) {
      return as_prioritized->size();
    }
    return as_fifo->size();
  }

private:
  std::unique_ptr<ThreadPoolFifo> as_fifo;
  std::unique_ptr<ThreadPoolWithPriority> as_prioritized;
  UniformSampler priority_sampler;
};
} // namespace CppThreadPool

TEST_CASE("Build destroy multiple times a pool", "[ThreadPool]") {
  const std::size_t size = 4;
  auto kind = GENERATE(true, false);
  for (std::size_t k = 0; k < 5; ++k) {
    CppThreadPool::PoolWrapper pool(size, kind);
    CHECK(size == pool.size());
  }
}

namespace {
class ExceptionTest : public std::runtime_error {
public:
  ExceptionTest() : ExceptionTest("") {}

  ExceptionTest(const std::string &message) : std::runtime_error(message) {}
};

template <std::size_t MILLISEC_WAIT> void wait() {
  std::this_thread::sleep_for(std::chrono::milliseconds{MILLISEC_WAIT});
}

void failure() { throw ExceptionTest{}; }
} // namespace

TEST_CASE("Single task completed notification", "[ThreadPool]") {
  auto threads = GENERATE(1, 2, 3, 4);
  auto kind = GENERATE(true, false);
  CppThreadPool::PoolWrapper pool(threads, kind);

  SECTION("Single push and single wait") {
    auto fut = pool.push(wait<500>);
    fut.wait();
    CHECK_NOTHROW(fut.get());
  }

  SECTION("Single push and single wait") {
    auto fut = pool.push(failure);
    fut.wait();
    CHECK_THROWS_AS(fut.get(), ExceptionTest);
  }
}

TEST_CASE("Multiple tasks completed notification", "[ThreadPool]") {
  auto threads = GENERATE(2, 4);
  auto kind = GENERATE(true, false);
  CppThreadPool::PoolWrapper pool(threads, kind);

  SECTION("Single push and single wait") {
    std::list<std::future<void>> waiters;
    for (std::size_t k = 0; k < threads; ++k) {
      waiters.emplace_back(pool.push(wait<500>));
    }
    for (auto &w : waiters) {
      w.wait();
      CHECK_NOTHROW(w.get());
    }
  }

  SECTION("Single push and single wait") {
    std::list<std::future<void>> waiters;
    for (std::size_t k = 0; k < threads; ++k) {
      waiters.emplace_back(pool.push(failure));
    }
    for (auto &w : waiters) {
      w.wait();
      CHECK_THROWS_AS(w.get(), ExceptionTest);
    }
  }
}

TEST_CASE("Multiple tasks and wait for all", "[ThreadPool]") {
  auto threads = GENERATE(2, 4);
  auto kind = GENERATE(true, false);
  CppThreadPool::PoolWrapper pool(threads, kind);

  for (std::size_t k = 0; k < 5; ++k) {
    std::list<std::future<void>> waiters;
    for (std::size_t k = 0; k < threads * 3; ++k) {
      waiters.emplace_back(pool.push(wait<500>));
    }
    pool.wait();
    for (auto &w : waiters) {
      CHECK_NOTHROW(w.get());
    }
  }
}

namespace {
std::chrono::milliseconds measure_time(const std::function<void()> &action) {
  auto tic = std::chrono::high_resolution_clock::now();
  action();
  return std::chrono::duration_cast<std::chrono::milliseconds>(
      std::chrono::high_resolution_clock::now() - tic);
}
} // namespace

TEST_CASE("Efficiency check", "[ThreadPool]") {
  const std::size_t tasks = 10;

  auto serial = measure_time([&]() {
    for (std::size_t t = 0; t < tasks; ++t) {
      wait<150>();
    }
  });

  CppThreadPool::ThreadPoolFifo pool(2);
  auto parallel = measure_time([&]() {
    for (std::size_t t = 0; t < tasks; ++t) {
      pool.push(wait<150>);
    }
    pool.wait();
  });

  CHECK(static_cast<double>(parallel.count()) <
        static_cast<double>(0.7 * serial.count()));
}
