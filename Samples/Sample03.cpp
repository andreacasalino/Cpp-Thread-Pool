/**
 * Author:    Andrea Casalino
 * Created:   25.09.2020
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <CppThreadPool/CppThreadPool.hxx>

#include <iostream>
#include <random>
using namespace std;

#define NUMBER_OF_SIMULATIONS 500
#define TRIAL_X_SIMULATION 500000
#define POOL_SIZE 4

// Pi (3.141...) computation through Monte Carlo  simulations.
//  Samples are drawn within the square [0,1] x [0,1] and the number of samples
//  inside the unitary radius circle centred in (0,0) is computed to determine
//  the Pi value. Results are stored in a Pi_computation object and queried
//  later.
class PiComputator {
public:
  PiComputator() : Trials_tot(0), Trials_inside(0){};

  double evalPi() {
    return 4.0 * (double)this->Trials_inside / (double)this->Trials_tot;
  };

  void newSimulation(const unsigned long &Trials) {
    unsigned int seed = (unsigned int)time(NULL);

    unsigned long int hits = 0;
    float sample[2], dist;
    for (size_t k = 0; k < Trials; ++k) {
#ifdef _WIN32
      // in Windows rand is tread safe
      sample[0] = (float)rand() / (float)RAND_MAX;
      sample[1] = (float)rand() / (float)RAND_MAX;
#else
      sample[0] = (float)rand_r(&seed) / (float)RAND_MAX;
      sample[1] = (float)rand_r(&seed) / (float)RAND_MAX;
#endif
      dist = sample[0] * sample[0] + sample[1] * sample[1];
      if (dist <= 1.f)
        ++hits;
    }

    this->Trials_tot += Trials;
    this->Trials_inside += hits;
  };

private:
  atomic<unsigned long> Trials_tot;
  atomic<unsigned long> Trials_inside;
};

int main() {

  // serial version
  {
    PiComputator Simulation;

    auto tic = chrono::steady_clock::now();

    Simulation.newSimulation(
        NUMBER_OF_SIMULATIONS *
        TRIAL_X_SIMULATION); // all the samples are drawn with a single call

    auto toc = chrono::steady_clock::now();

    cout << "Elapsed time with a single thread: "
         << chrono::duration_cast<chrono::milliseconds>(toc - tic).count()
         << endl;
    cout << "Pi value: " << Simulation.evalPi() << endl;
  }

  // pool of threads
  {
    PiComputator Simulation;

    auto tic = chrono::steady_clock::now();

    CppThreadPool::Fifo pool(POOL_SIZE);
    for (unsigned long k = 0; k < NUMBER_OF_SIMULATIONS; ++k)
      pool.push(
          [&Simulation]() { Simulation.newSimulation(TRIAL_X_SIMULATION); });
    pool.wait(); // wait for the pool to terminate the computations

    auto toc = chrono::steady_clock::now();

    cout << endl
         << "Elapsed time with a thread pool of size " << POOL_SIZE << ": "
         << chrono::duration_cast<chrono::milliseconds>(toc - tic).count()
         << endl;
    cout << "Pi value: " << Simulation.evalPi() << endl;
  }

  return EXIT_SUCCESS;
}
