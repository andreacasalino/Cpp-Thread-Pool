/**
* Author:    Andrea Casalino
* Created:   25.09.2020
*
* report any bug to andrecasa91@gmail.com.
**/

#include <EquiPool.h>
#include <iostream>
#include <chrono>
using namespace std;

#define SLEEP_TIME 200
#define TASK_NUMBER 50

// make the current wait for a little bit
function<void(void)> make_wait(){
    return [](){ 
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_TIME)); 
    };
}

int main(){

//serial version
{
    auto tic = chrono::steady_clock::now();

    for(auto k=0; k<TASK_NUMBER; ++k) make_wait()();

    auto toc = chrono::steady_clock::now();
    cout << endl << "Elapsed time with a single thread: " << chrono::duration_cast<chrono::milliseconds>(toc- tic).count() << endl;
}

//thread pool version
{
    auto tic = chrono::steady_clock::now();

    thpl::equi::Pool P(4);
    for(auto k=0; k<TASK_NUMBER; ++k) P.push(make_wait());
    P.wait();

    auto toc = chrono::steady_clock::now();
    cout << endl << "Elapsed time with the thread pool: " << chrono::duration_cast<chrono::milliseconds>(toc- tic).count() << endl;
}

    return 0;
}
