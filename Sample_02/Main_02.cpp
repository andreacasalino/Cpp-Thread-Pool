/**
* Author:    Andrea Casalino
* Created:   06.08.2020
*
* report any bug to andrecasa91@gmail.com.
**/

#include "../src/Thread_pool.h"

#include <chrono>
#include <iostream>
#include <list>
using namespace std;

#define NUMBER_OF_SIMULATIONS 500
#define TRIAL_X_SIMULATION 500000
#define POOL_SIZE 4

//Pi computation through Monte Carlo  simulations.
// Samples are drawn whithin [0,1]X[0,1] and the number of samples inside the unitary radius circle centred in (0,0) is computed
// to determine the Pi value.
// Results are stored in a Pi_computation object and queried later.
class Pi_computation{
public:
    Pi_computation() : Trials_tot(0), Trials_inside(0) { if (pthread_mutex_init(&this->result_mtx, NULL) != 0) throw 1; };

    class Simulation{
    public:
        Simulation(const unsigned long int& trials, Pi_computation* source) : Trials(trials), Source(source){ };

        void operator()(){ 

            float sample[2];
            unsigned long int N_inside = 0;
            unsigned int seed = (unsigned int)rand();
            for(unsigned long int k=0; k<this->Trials; ++k){
                sample[0] = 2.f * (float)rand_r(&seed) / (float)RAND_MAX - 1.f;
                sample[1] = 2.f * (float)rand_r(&seed) / (float)RAND_MAX - 1.f;
                if( (sample[0]*sample[0] + sample[1]*sample[1]) <= 1.f ) ++N_inside;
            }
            pthread_mutex_lock(&this->Source->result_mtx);
            this->Source->Trials_tot += this->Trials;
            this->Source->Trials_inside += N_inside;
            pthread_mutex_unlock(&this->Source->result_mtx);

        };
    private:
        unsigned long int       Trials;
        Pi_computation*         Source;
    };

    double eval_Pi() { return 4.0 * (double)this->Trials_inside / (double)this->Trials_tot; };
private:
    pthread_mutex_t             result_mtx;
    unsigned long int           Trials_tot;
    unsigned long int           Trials_inside;
};

int main(){

// serial version
{
    Pi_computation Simulation;

    auto tic = chrono::steady_clock::now();

    Pi_computation::Simulation serial_trials((unsigned long int)(NUMBER_OF_SIMULATIONS * TRIAL_X_SIMULATION), &Simulation); //all the samples are drawn with a single call 
    serial_trials();

    auto toc = chrono::steady_clock::now();

    cout << endl << "Elapsed time with a single thread: " << chrono::duration_cast<chrono::milliseconds>(toc- tic).count() << endl;
    cout << endl << "Pi value: " << Simulation.eval_Pi() << endl;
}


//pool of threads
{
    Pi_computation Simulation;

    auto tic = chrono::steady_clock::now();

    Pool_equipriority pool(POOL_SIZE); 
    for(unsigned long int k=0; k<NUMBER_OF_SIMULATIONS; ++k)
        pool.push(new Pi_computation::Simulation(TRIAL_X_SIMULATION , &Simulation)); //since for destroy_after_process is implicitly assumed true, the functor will be destroyed interally by the pool after having being processed 
    pool.await(); //wait for the pool to terminate the computations

    auto toc = chrono::steady_clock::now();

    cout << endl << "Elapsed time with a thread pool: " << chrono::duration_cast<chrono::milliseconds>(toc- tic).count() << endl;
    cout << endl << "Pi value: " << Simulation.eval_Pi() << endl;
}

    return 0;

}
