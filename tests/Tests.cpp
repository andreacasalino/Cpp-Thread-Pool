#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <CppThreadPool/CppThreadPool.hxx>

#include <random>
#include <stdexcept>
#include <variant>

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

enum class PoolType { FIFO, LIFO, PRIORITY };

using AnyPool = std::variant<std::unique_ptr<Fifo>, std::unique_ptr<Lifo>,
                             std::unique_ptr<Prioritized>>;

class PoolWrapper {
public:
  PoolWrapper(const std::size_t size, const PoolType type) {
    switch (type) {
    case PoolType::FIFO:
      pool = std::make_unique<Fifo>(size);
      break;
    case PoolType::LIFO:
      pool = std::make_unique<Lifo>(size);
      break;
    case PoolType::PRIORITY:
      pool = std::make_unique<Prioritized>(size);
      break;
    }
  }

  std::future<void> push(const std::function<void()> &action) {
    std::future<void> result;
    struct Visitor {
      const std::function<void()> &action;
      std::future<void> &result;
      UniformSampler &priority_sampler;

      void operator()(std::unique_ptr<Fifo> &pool) const {
        result = pool->push(action);
      }
      void operator()(std::unique_ptr<Lifo> &pool) const {
        result = pool->push(action);
      }
      void operator()(std::unique_ptr<Prioritized> &pool) const {
        Priority p = priority_sampler.sample();
        result = pool->push(action, p);
      }
    } visitor{action, result, priority_sampler};
    std::visit(visitor, pool);
    return result;
  }

  void wait() {
    struct Visitor {
      void operator()(std::unique_ptr<Fifo> &pool) const { pool->wait(); }
      void operator()(std::unique_ptr<Lifo> &pool) const { pool->wait(); }
      void operator()(std::unique_ptr<Prioritized> &pool) const {
        pool->wait();
      }
    } visitor;
    std::visit(visitor, pool);
  }

  std::size_t size() const {
    std::size_t result;
    struct Visitor {
      std::size_t &result;

      void operator()(const std::unique_ptr<Fifo> &pool) const {
        result = pool->size();
      }
      void operator()(const std::unique_ptr<Lifo> &pool) const {
        result = pool->size();
      }
      void operator()(const std::unique_ptr<Prioritized> &pool) const {
        result = pool->size();
      }
    } visitor{result};
    std::visit(visitor, pool);
    return result;
  }

private:
  AnyPool pool;
  UniformSampler priority_sampler;
};
} // namespace CppThreadPool

#define GENERATE_POOL_TYPES                                                    \
  GENERATE(CppThreadPool::PoolType::FIFO, CppThreadPool::PoolType::LIFO,       \
           CppThreadPool::PoolType::PRIORITY)

TEST_CASE("Build destroy multiple times a pool", "[ThreadPool]") {
  const std::size_t size = 4;
  auto kind = GENERATE_POOL_TYPES;
  for (std::size_t k = 0; k < 5; ++k) {
    CppThreadPool::PoolWrapper pool(size, kind);
    CHECK(size == pool.size());
  }
}

namespace {
template <std::size_t MILLISEC_WAIT> void wait() {
  std::this_thread::sleep_for(std::chrono::milliseconds{MILLISEC_WAIT});
}

class ExceptionTest : public std::runtime_error {
public:
  ExceptionTest() : std::runtime_error("") {}
};

void failure() { throw ExceptionTest{}; }
} // namespace

TEST_CASE("Single task completed notification", "[ThreadPool]") {
  auto threads = GENERATE(1, 2, 3, 4);
  auto kind = GENERATE_POOL_TYPES;
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
  auto kind = GENERATE_POOL_TYPES;
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
  auto kind = GENERATE_POOL_TYPES;
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

  CppThreadPool::Fifo pool(2);
  auto parallel = measure_time([&]() {
    for (std::size_t t = 0; t < tasks; ++t) {
      pool.push(wait<150>);
    }
    pool.wait();
  });

  CHECK(static_cast<double>(parallel.count()) <
        static_cast<double>(0.7 * serial.count()));
}
