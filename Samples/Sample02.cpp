/**
* Author:    Andrea Casalino
* Created:   25.09.2020
*
* report any bug to andrecasa91@gmail.com.
**/

#include <EquiPool.h>

#include <chrono>
#include <iostream>
#include <fstream>
#include <vector>
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
    Mean_computator(const Mean_computator& ) = delete;

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

    vector<float> meanSerial;

//serial version (may take a while)
{
    auto tic = chrono::steady_clock::now();

    for(auto it=work_packages.begin(); it!=work_packages.end(); ++it) (*it)();

    auto toc = chrono::steady_clock::now();
    cout << endl << "Elapsed time with a single thread: " << chrono::duration_cast<chrono::milliseconds>(toc- tic).count() << endl;

    // save the results in order to compare it later with the ones obtained with the pool of threads
    meanSerial.reserve(work_packages.size());
    for (auto it = work_packages.begin(); it != work_packages.end(); ++it) meanSerial.push_back(it->get_computed_mean());

}

//pool of threads
{
    auto tic = chrono::steady_clock::now();

    thpl::equi::Pool pool(POOL_SIZE); //After construction the threads are already spawned and ready to process work package
    for (list<Mean_computator>::iterator it = work_packages.begin(); it != work_packages.end(); ++it) pool.push([it]() { (*it)(); });
    pool.wait(); //wait for the pool to terminate the tasks

    auto toc = chrono::steady_clock::now();
    cout << endl << "Elapsed time with the thread pool: " << chrono::duration_cast<chrono::milliseconds>(toc- tic).count() << endl;

    // print the computed means in the same file. Compare it to convince yourself that the pool of threads did the same computations of the serial version.
    ofstream f("Results");
    f << "serial   parallel\n";
    size_t r = 0;
    for (auto it = work_packages.begin(); it != work_packages.end(); ++it) {
        f << meanSerial[r] << "      " << it->get_computed_mean() << endl;
        ++r;
    }
    f.close();
}

    return 0;
}
