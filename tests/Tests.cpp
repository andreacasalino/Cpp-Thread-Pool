#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include <CppThreadPool/CppThreadPool.hxx>

TEST_CASE("Single task completed notification", "[ThreadPool]") {
  auto threads = GENERATE(2, 3, 4);

  CppThreadPool::ThreadPoolFifo pool(threads);

  auto fut = pool.push(
      [&]() { std::this_thread::sleep_for(std::chrono::seconds{1}); });
  fut.wait();
}
