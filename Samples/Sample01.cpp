/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <CppThreadPool/CppThreadPool.hxx>

#include <iostream>
using namespace std;

#define SLEEP_TIME 200
#define TASK_NUMBER 50

// make the current wait for a little bit
void wait() {
  std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME));
}

int main() {

  // thread pool version
  {
    auto tic = chrono::steady_clock::now();

    CppThreadPool::ThreadPoolFifo P(4);

    for (auto k = 0; k < TASK_NUMBER; ++k) {
      P.push(wait);
    }
    P.wait();

    auto toc = chrono::steady_clock::now();
    cout << endl
         << "Elapsed time with the thread pool: "
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
