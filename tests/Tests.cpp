#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <CppThreadPool/CppThreadPool.hxx>

#include "UniformSampler.h"

#include <stdexcept>
#include <vector>

using namespace CppThreadPool;

TEST_CASE("Build destroy multiple times a pool", "[ThreadPool]") {
  const std::size_t size = 4;

  for (std::size_t k = 0; k < 5; ++k) {
    Fifo pool(size);
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

void throw_exc() { throw ExceptionTest{}; }
} // namespace

TEST_CASE("Single task completed notification", "[ThreadPool]") {
  auto threads = GENERATE(1, 2, 3, 4);
  Fifo pool{static_cast<std::size_t>(threads)};

  SECTION("Single push and single wait, success") {
    auto fut = pool.push(wait<500>);
    fut.wait();
    CHECK_NOTHROW(fut.get());
  }

  SECTION("Single push and single wait, failure") {
    auto fut = pool.push(throw_exc);
    fut.wait();
    CHECK_THROWS_AS(fut.get(), ExceptionTest);
  }
}

TEST_CASE("Multiple tasks completed notification", "[ThreadPool]") {
  auto threads = GENERATE(2, 4);
  Fifo pool{static_cast<std::size_t>(threads)};

  SECTION("Multiple push and single wait, success") {
    std::vector<std::future<void>> waiters;
    for (std::size_t k = 0; k < threads; ++k) {
      waiters.emplace_back(pool.push(wait<500>));
    }
    for (auto &w : waiters) {
      w.wait();
      CHECK_NOTHROW(w.get());
    }
  }

  SECTION("Multiple push and single wait, failure") {
    std::vector<std::future<void>> waiters;
    for (std::size_t k = 0; k < threads; ++k) {
      waiters.emplace_back(pool.push(throw_exc));
    }
    for (auto &w : waiters) {
      w.wait();
      CHECK_THROWS_AS(w.get(), ExceptionTest);
    }
  }
}

TEST_CASE("Multiple tasks and wait for all", "[ThreadPool]") {
  auto threads = GENERATE(2, 4);
  Fifo pool{static_cast<std::size_t>(threads)};

  for (std::size_t k = 0; k < 5; ++k) {
    std::vector<std::future<void>> waiters;
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
template <typename Pred> std::chrono::microseconds measure_time(Pred action) {
  auto tic = std::chrono::high_resolution_clock::now();
  action();
  return std::chrono::duration_cast<std::chrono::microseconds>(
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

  Fifo pool(2);
  auto parallel = measure_time([&]() {
    for (std::size_t t = 0; t < tasks; ++t) {
      pool.push(wait<150>);
    }
    pool.wait();
  });

  CHECK(static_cast<double>(parallel.count()) <
        static_cast<double>(0.7 * serial.count()));
}

// TODO check other pool types
