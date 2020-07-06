#include "../src/Thread_pool.h"

#include <chrono>
#include <iostream>
#include <fstream>
#include <list>
using namespace std;

#define NUMBER_OF_ARRAYS 50
#define ARRAY_SIZE 5000000
#define POOL_SIZE 4

//sample a buffer of randomic numbers, whose mean must be computed later
class Mean_computator {
public:
    Mean_computator(const size_t& N_samples) : values_size(N_samples) {
        this->values = new int[N_samples];
        for(size_t k=0; k<N_samples; ++k) this->values[k] = -10 + rand() % 20;
    };
    ~Mean_computator(){ delete[] this->values; };

    void operator()(){
        this->Mean = 0.f;
        for(size_t k=0; k<this->values_size; ++k) this->Mean += this->values[k];
        this->Mean *= 1.f / (float)this->values_size;
    };

    const float&  get_computed_mean() const { return this->Mean; };
private:
    size_t      values_size;
    int*        values;
    float       Mean;
};

int main(){

// build some tasks to to

    list<Mean_computator>  work_packages;
    for(int k=0; k<NUMBER_OF_ARRAYS; ++k){
        work_packages.emplace_back(ARRAY_SIZE);
    }

//serial version (may take a while)
{
    auto tic = chrono::steady_clock::now();

    for(auto it=work_packages.begin(); it!=work_packages.end(); ++it) (*it)();

    auto toc = chrono::steady_clock::now();
    cout << endl << "Elapsed time with a single thread: " << chrono::duration_cast<chrono::milliseconds>(toc- tic).count() << endl;

    // print the computed means in a textual file
    ofstream f("Results");
    f << "means computed with the serial version : ";
    for(auto it=work_packages.begin(); it!=work_packages.end(); ++it) f << " " << it->get_computed_mean();
    f.close();

}

//pool of threads
{
    auto tic = chrono::steady_clock::now();

    Pool_equipriority pool(POOL_SIZE); //default polling time assumed. After construction the threads are already spawned and ready to process work package
    for(auto it=work_packages.begin(); it!=work_packages.end(); ++it) pool.push(&(*it), false); // false is passed to instruct the pool to not destroy the functor after having processed it
    pool.await(); //wait for the pool to terminate the tasks

    auto toc = chrono::steady_clock::now();
    cout << endl << "Elapsed time with the thread pool: " << chrono::duration_cast<chrono::milliseconds>(toc- tic).count() << endl;

    // print the computed means in the same file. Compare it to convince yourself that the pool of threads did the same computations of the serial version.
    ofstream f("Results", ios_base::app);
    f << "\nmeans computed with the thread pool     : ";
    for(auto it=work_packages.begin(); it!=work_packages.end(); ++it) f << " " << it->get_computed_mean();
    f.close();
}

    return 0;
}
