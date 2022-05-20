#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <CppThreadPool/CppThreadPool.hxx>

#include <stdexcept>

TEST_CASE("Build destroy multiple times a pool", "[ThreadPool]") {
  const std::size_t size = 4;
  for (std::size_t k = 0; k < 5; ++k) {
    CppThreadPool::ThreadPoolFifo pool(size);
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

  CppThreadPool::ThreadPoolFifo pool(threads);

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

  CppThreadPool::ThreadPoolFifo pool(threads);

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

  CppThreadPool::ThreadPoolFifo pool(threads);

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
