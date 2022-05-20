/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <CppThreadPool/CppThreadPool.hxx>

#include <iostream>
using namespace std;

static constexpr std::size_t TASK_NUMBER = 50;

// make the current wait for a little bit
void wait() { std::this_thread::sleep_for(std::chrono::milliseconds{200}); }

int main() {

  // thread pool version
  {
    auto tic = chrono::steady_clock::now();

    const std::size_t pool_size = 4;
    CppThreadPool::ThreadPoolFifo P(pool_size);

    for (auto k = 0; k < TASK_NUMBER; ++k) {
      P.push(wait);
    }
    P.wait();

    auto toc = chrono::steady_clock::now();
    cout << endl
         << "Elapsed time with the thread pool of size " << pool_size << ": "
         << chrono::duration_cast<chrono::milliseconds>(toc - tic).count()
         << " ms" << endl;
  }

  // serial version
  {
    auto tic = chrono::steady_clock::now();

    for (auto k = 0; k < TASK_NUMBER; ++k) {
      wait();
    }

    auto toc = chrono::steady_clock::now();
    cout << endl
         << "Elapsed time with a single thread: "
         << chrono::duration_cast<chrono::milliseconds>(toc - tic).count()
         << " ms" << endl;
  }

  return EXIT_SUCCESS;
}
